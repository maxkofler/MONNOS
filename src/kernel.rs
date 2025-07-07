#![no_std]
#![no_main]
#![allow(dead_code)]
#![feature(sync_unsafe_cell)]

extern crate alloc;

use core::arch::asm;

use alloc::{boxed::Box, vec::Vec};
use log::{debug, info, Level, LevelFilter};
use monnos::{
    drivers::uart::{BlockingUART, COM_BASE},
    interrupts,
    io::pci::{self, configuration::PCIHeader, strings::get_pci_class_string},
    log::MONNOSLogger,
    memory::{set_memory_area, MemoryArea},
    multiboot::info::{memory_map::MemoryRegion, parse_boot_info},
};

pub mod monnos;

fn get_first_uart() -> Option<(usize, BlockingUART)> {
    for (num, base) in COM_BASE.iter().enumerate() {
        if let Some(uart) = BlockingUART::new(*base) {
            return Some((num, uart));
        }
    }

    None
}

#[no_mangle]
extern "C" fn kernel_entry(pos_multiboot_info: u32) -> ! {
    let (_com_num, uart) = get_first_uart().expect("Some working UART");

    let logger = MONNOSLogger::new(uart, Level::Trace);
    let logger: &'static MONNOSLogger = unsafe { core::mem::transmute(&logger) };

    log::set_logger(logger)
        .map(|()| log::set_max_level(LevelFilter::Trace))
        .expect("Setup logger");

    for (num, base) in COM_BASE.iter().enumerate() {
        if BlockingUART::new(*base).is_some() {
            debug!("Found working UART COM{num}");
        }
    }

    let multiboot_info = parse_boot_info(pos_multiboot_info);
    let cmdline = multiboot_info.cmdline.unwrap_or("");
    info!("Kernel cmdline: '{cmdline}'");

    for module in multiboot_info.modules.unwrap_or(&[]) {
        let module = module.parse();
        info!("Found multiboot module '{}'", module.string,);
    }

    let mut largest_area: Option<MemoryRegion> = None;

    if let Some(memory_map) = multiboot_info.memory_map {
        for mut entry in memory_map.iter() {
            if let Some(entry) = entry.take() {
                debug!("Memory region: {entry:x?} ({} bytes)", entry.data.len());

                if let Some(ref largest) = largest_area {
                    if largest.data.len() < entry.data.len() {
                        largest_area.replace(entry);
                    }
                } else {
                    largest_area.replace(entry);
                }
            }
        }
    }

    if let Some(largest) = largest_area {
        debug!(
            "Using {largest:x?} ({} bytes) as main region",
            largest.data.len()
        );

        unsafe {
            set_memory_area(MemoryArea::new(
                largest.data.as_ptr() as *mut (),
                largest.data.len(),
            ))
        };
    }

    interrupts::enable_interrupts();

    pci::enumerate_simple(|bus_number, device_number, header| {
        if let PCIHeader::Device(device) = header {
            debug!(
                "{:x}:{:x} Found PCI device: {} (PI: {})",
                bus_number,
                device_number,
                get_pci_class_string(device.class, device.subclass, device.programming_interface),
                device.programming_interface
            );
        }
    });

    debug!("PCI Enumeration done");

    info!("Monnos is ready!");

    #[allow(clippy::empty_loop)]
    loop {
        unsafe { asm!("hlt") };
    }
}
