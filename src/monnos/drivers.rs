//! All the drivers in the MONNOS operating system

use alloc::{boxed::Box, vec::Vec};

use super::io::pci::configuration::PCIDeviceHeader;

pub mod uart;

pub mod network;

/// An interface that can be used to interact with
/// a device
pub enum DeviceInterface {
    /// PCI
    PCI(PCIDeviceHeader),
}

/// The common trait for all drivers
pub trait Driver {
    /// Initialize and create a new instance of this driver
    /// using the supplied interface
    /// # Arguments
    /// * `interface` - The interface to use for talking to the device
    fn instantiate(interface: &DeviceInterface) -> Self
    where
        Self: Sized;
}

/// A factory that can create driver instances from interfaces
pub trait DriverFactory {
    /// Try to instantiate drivers from the interface
    fn try_instantiate(interface: &DeviceInterface) -> Vec<Box<dyn Driver>>;
}
