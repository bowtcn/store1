// license:BSD-3-Clause
// copyright-holders:R. Belmont,Kevin Horton
/***************************************************************************

    MPU-401 MIDI device interface

***************************************************************************/

#include "emu.h"
#include "mpu_pc98.h"

#define MPU_CORE_TAG "mpu401"


/*
DIP-SWs
1-2-3-4
         0xc0d0
      1  0xc4d0
    1    0xc8d0
...
1        0xe0d0 (default)
...
1 1 1 1  0xfcd0

5-6-7-8
1        irq12
  1      irq6 (default)
    1    irq5
      1  irq3
*/

WRITE_LINE_MEMBER( mpu_pc98_device::mpu_irq_out )
{
}

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(MPU_PC98, mpu_pc98_device, "mpu_pc98", "Roland MPU-401 MIDI Interface (CBUS)")

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

MACHINE_CONFIG_START(mpu_pc98_device::device_add_mconfig)
	MCFG_MPU401_ADD(MPU_CORE_TAG, WRITELINE(*this, mpu_pc98_device, mpu_irq_out))
MACHINE_CONFIG_END


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

mpu_pc98_device::mpu_pc98_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, MPU_PC98, tag, owner, clock)
	, m_bus(*this, DEVICE_SELF_OWNER)
	, m_mpu401(*this, MPU_CORE_TAG)
{
}

void mpu_pc98_device::map(address_map &map)
{
	map(0x0, 0x0).rw(MPU_CORE_TAG, FUNC(mpu401_device::mpu_r), FUNC(mpu401_device::mpu_w));
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void mpu_pc98_device::device_start()
{
	address_space &iospace = m_bus->io_space();
	iospace.install_device(0xe0d0, 0xe0d3, *this, &mpu_pc98_device::map);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void mpu_pc98_device::device_reset()
{
}
