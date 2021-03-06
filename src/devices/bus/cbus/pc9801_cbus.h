// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/**********************************************************************


**********************************************************************/
#ifndef MAME_MACHINE_PC9801_CBUS_H
#define MAME_MACHINE_PC9801_CBUS_H

#pragma once




//**************************************************************************
//  CONSTANTS
//**************************************************************************



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_PC9801CBUS_CPU(_cputag) \
	downcast<pc9801_slot_device &>(*device).set_cpu_tag(_cputag);

#define MCFG_PC9801CBUS_INT0_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<0>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT1_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<1>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT2_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<2>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT3_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<3>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT4_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<4>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT5_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<5>(DEVCB_##_devcb);

#define MCFG_PC9801CBUS_INT6_CALLBACK(_devcb) \
	downcast<pc9801_slot_device &>(*device).set_int_callback<6>(DEVCB_##_devcb);


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************




//class pc9801_slot_device;

#if 0
class device_pc9801_slot_card_interface : public device_slot_card_interface
{
	friend class pc9801_slot_device;

public:
	// construction/destruction
	device_pc9801_slot_card_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_pc9801_card_interface();
};
#endif

// ======================> pc9801_slot_device

class pc9801_slot_device : public device_t, public device_slot_interface
{
public:
	// construction/destruction
	template <typename T, typename U>
	pc9801_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&cpu_tag, U &&opts, char const *dflt)
		: pc9801_slot_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_cpu.set_tag(std::forward<T>(cpu_tag));
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	pc9801_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration access
	template<std::size_t Line> auto int_cb() { return m_int_callback[Line].bind(); }

	address_space &program_space() const { return m_cpu->space(AS_PROGRAM); }
	address_space &io_space() const { return m_cpu->space(AS_IO); }
	template<int I> void int_w(bool state) { m_int_callback[I](state); }
	void install_io(offs_t start, offs_t end, read8_delegate rhandler, write8_delegate whandler);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_config_complete() override;
	virtual void device_resolve_objects() override;

private:
//  device_pc9801_slot_card_interface *m_card;
	required_device<cpu_device> m_cpu;
	devcb_write_line m_int_callback[7];
};


// device type definition
DECLARE_DEVICE_TYPE(PC9801CBUS_SLOT, pc9801_slot_device)

#endif // MAME_MACHINE_PC9801_CBUS_H
