// license:BSD-3-Clause
// copyright-holders:Luca Elia, Mirko Buffoni, Takahiro Nogi

#pragma once

#ifndef MAME_INCLUDES_TNZS_H
#define MAME_INCLUDES_TNZS_H

#include "sound/dac.h"
#include "sound/samples.h"
#include "video/seta001.h"
#include "cpu/mcs48/mcs48.h"
#include "machine/bankdev.h"
#include "machine/gen_latch.h"
#include "machine/upd4701.h"

#define MAX_SAMPLES 0x2f        /* max samples */

class tnzs_base_state : public driver_device
{
public:
	tnzs_base_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_subcpu(*this, "sub")
		, m_seta001(*this, "spritegen")
		, m_palette(*this, "palette")
		, m_mainbank(*this, "mainbank")
		, m_subbank(*this, "subbank")
	{ }

	virtual void machine_start() override;

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w);

	DECLARE_WRITE8_MEMBER(ramrom_bankswitch_w);

	uint32_t screen_update_tnzs(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank_tnzs);

	DECLARE_PALETTE_INIT(prompalette);

	void tnzs_base(machine_config &config);
	void tnzs_mainbank(machine_config &config);

	void base_sub_map(address_map &map);
	void main_map(address_map &map);
	void mainbank_map(address_map &map);
protected:
	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_subcpu;
	optional_device<seta001_device> m_seta001;
	required_device<palette_device> m_palette;
	optional_device<address_map_bank_device> m_mainbank; /* FIXME: optional because of reuse from cchance.cpp */
	optional_memory_bank m_subbank; /* FIXME: optional because of reuse from cchance.cpp */

	/* misc / mcu */
	int      m_bank2;
};

class tnzs_mcu_state : public tnzs_base_state
{
public:
	tnzs_mcu_state(const machine_config &mconfig, device_type type, const char *tag, bool lockout_level)
		: tnzs_base_state(mconfig, type, tag)
		, m_mcu(*this, "mcu")
		, m_upd4701(*this, "upd4701")
		, m_in0(*this, "IN0")
		, m_in1(*this, "IN1")
		, m_in2(*this, "IN2")
		, m_input_select(0)
		, m_lockout_level(lockout_level)
	{ }

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w) override;

	DECLARE_READ8_MEMBER(mcu_port1_r);
	DECLARE_READ8_MEMBER(mcu_port2_r);
	DECLARE_WRITE8_MEMBER(mcu_port2_w );
	DECLARE_READ8_MEMBER(mcu_r);
	DECLARE_WRITE8_MEMBER(mcu_w);

	DECLARE_READ8_MEMBER(analog_r);

	void tnzs(machine_config &config);
	void tnzs_sub_map(address_map &map);
protected:
	required_device<upi41_cpu_device> m_mcu;
	optional_device<upd4701_device> m_upd4701;

	required_ioport m_in0;
	required_ioport m_in1;
	required_ioport m_in2;

	int      m_input_select;
	bool     m_lockout_level;
};

class tnzs_state : public tnzs_mcu_state
{
public:
	tnzs_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_mcu_state(mconfig, type, tag, true)
	{ }
};

class extrmatn_state : public tnzs_mcu_state
{
public:
	extrmatn_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_mcu_state(mconfig, type, tag, false)
	{ }
	void extrmatn(machine_config &config);
	void plumppop(machine_config &config);
	void prompal_main_map(address_map &map);
};

class arknoid2_state : public extrmatn_state
{
public:
	arknoid2_state(const machine_config &mconfig, device_type type, const char *tag)
		: extrmatn_state(mconfig, type, tag)
		, m_coin1(*this, "COIN1")
		, m_coin2(*this, "COIN2")
		, m_in0(*this, "IN0")
		, m_in1(*this, "IN1")
		, m_in2(*this, "IN2")
	{ }

	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w) override;

	DECLARE_READ8_MEMBER(mcu_r);
	DECLARE_WRITE8_MEMBER(mcu_w);
	INTERRUPT_GEN_MEMBER(mcu_interrupt);

	void arknoid2(machine_config &config);
	void arknoid2_sub_map(address_map &map);
private:
	required_ioport m_coin1;
	required_ioport m_coin2;
	required_ioport m_in0;
	required_ioport m_in1;
	required_ioport m_in2;

	void mcu_reset();

	int      m_mcu_initializing;
	int      m_mcu_coinage_init;
	int      m_mcu_command;
	int      m_mcu_readcredits;
	int      m_mcu_reportcoin;
	int      m_insertcoin;
	uint8_t    m_mcu_coinage[4];
	uint8_t    m_mcu_coins_a;
	uint8_t    m_mcu_coins_b;
	uint8_t    m_mcu_credits;

	void mcu_handle_coins(int coin);
};

class kageki_state : public tnzs_base_state
{
public:
	kageki_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_base_state(mconfig, type, tag)
		, m_samples(*this, "samples")
		, m_dswa(*this, "DSWA")
		, m_dswb(*this, "DSWB")
		, m_csport_sel(0)
	{ }

	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w) override;

	DECLARE_READ8_MEMBER(csport_r);
	DECLARE_WRITE8_MEMBER(csport_w);

	void machine_reset_kageki();
	void init_kageki();

	SAMPLES_START_CB_MEMBER(init_samples);

	void kageki(machine_config &config);
	void kageki_sub_map(address_map &map);
private:
	required_device<samples_device> m_samples;

	required_ioport m_dswa;
	required_ioport m_dswb;

	/* sound-related */
	std::unique_ptr<int16_t[]>    m_sampledata[MAX_SAMPLES];
	int      m_samplesize[MAX_SAMPLES];

	int      m_csport_sel;
};

class jpopnics_state : public tnzs_base_state
{
public:
	jpopnics_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_base_state(mconfig, type, tag)
		, m_upd4701(*this, "upd4701")
	{ }

	virtual void machine_reset() override;

	DECLARE_WRITE8_MEMBER(subbankswitch_w);

	void jpopnics(machine_config &config);
	void jpopnics_main_map(address_map &map);
	void jpopnics_sub_map(address_map &map);
private:
	required_device<upd4701_device> m_upd4701;
};

class insectx_state : public tnzs_base_state
{
public:
	insectx_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_base_state(mconfig, type, tag)
	{ }

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w) override;
	void insectx(machine_config &config);
	void insectx_sub_map(address_map &map);
};

class tnzsb_state : public tnzs_base_state
{
public:
	tnzsb_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzs_base_state(mconfig, type, tag)
		, m_audiocpu(*this, "audiocpu")
		, m_soundlatch(*this, "soundlatch")
	{ }

	DECLARE_WRITE_LINE_MEMBER(ym2203_irqhandler);

	DECLARE_WRITE8_MEMBER(sound_command_w);

	virtual DECLARE_WRITE8_MEMBER(bankswitch1_w) override;

	void tnzsb(machine_config &config);
	void tnzsb_base_sub_map(address_map &map);
	void tnzsb_cpu2_map(address_map &map);
	void tnzsb_io_map(address_map &map);
	void tnzsb_main_map(address_map &map);
	void tnzsb_sub_map(address_map &map);
protected:
	required_device<cpu_device> m_audiocpu;
	required_device<generic_latch_8_device> m_soundlatch;
};

class kabukiz_state : public tnzsb_state
{
public:
	kabukiz_state(const machine_config &mconfig, device_type type, const char *tag)
		: tnzsb_state(mconfig, type, tag)
		, m_audiobank(*this, "audiobank")
	{ }

	virtual void machine_start() override;

	DECLARE_WRITE8_MEMBER(sound_bank_w);

	void kabukiz(machine_config &config);
	void kabukiz_cpu2_map(address_map &map);
	void kabukiz_sub_map(address_map &map);
protected:
	required_memory_bank m_audiobank;
};

#endif // MAME_INCLUDES_TNZS_H
