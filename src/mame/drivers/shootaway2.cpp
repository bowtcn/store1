// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

    Namco M74 board - "Shoot Away II"

    Skeleton driver by R. Belmont

    Main CPU: ROMless Mitsubishi M37450 rebadged as Namco C68 custom
    I/O CPU: TMPZ84C011

    M37450 needs on-board timers implemented to go anywhere
    (see Mitsu '89 single-chip CPU databook on Bitsavers)

****************************************************************************/

#include "emu.h"
#include "cpu/z80/tmpz84c011.h"
#include "cpu/m6502/m3745x.h"
#include "sound/okim6295.h"
#include "machine/nvram.h"
#include "screen.h"
#include "speaker.h"

class m74_state : public driver_device
{
public:
	m74_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_subcpu(*this, "subcpu")
	{ }

	virtual void machine_reset() override;
	virtual void machine_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void m74(machine_config &config);
	void c68_map(address_map &map);
	void sub_map(address_map &map);
protected:

	// devices
	required_device<m37450_device> m_maincpu;
	required_device<tmpz84c011_device> m_subcpu;

	// driver_device overrides
	virtual void video_start() override;
};

void m74_state::machine_reset()
{
}

void m74_state::machine_start()
{
}

void m74_state::video_start()
{
}

uint32_t m74_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return 0;
}

void m74_state::c68_map(address_map &map)
{
	map(0x8000, 0xffff).rom().region("maincpu", 0x0000);
}

void m74_state::sub_map(address_map &map)
{
	map(0x0000, 0x7fff).rom().region("subcpu", 0);
	map(0x8000, 0xffff).ram();
}

static INPUT_PORTS_START( m74 )
INPUT_PORTS_END

MACHINE_CONFIG_START(m74_state::m74)
	MCFG_DEVICE_ADD("maincpu", M37450, XTAL(8'000'000)) /* C68 @ 8.0MHz - main CPU */
	MCFG_DEVICE_PROGRAM_MAP(c68_map)

	MCFG_DEVICE_ADD("subcpu", TMPZ84C011, XTAL(12'000'000) / 3)  /* Z84C011 @ 4 MHz - sub CPU */
	MCFG_DEVICE_PROGRAM_MAP(sub_map)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MCFG_SCREEN_UPDATE_DRIVER(m74_state, screen_update)
	MCFG_SCREEN_SIZE(320, 240)
	MCFG_SCREEN_VISIBLE_AREA(0, 319, 0, 239)

	SPEAKER(config, "mono").front_center();
	MCFG_DEVICE_ADD("oki", OKIM6295, XTAL(1'000'000), okim6295_device::PIN7_HIGH)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END

ROM_START(shootaw2)
	ROM_REGION(0x20000, "maincpu", 0)       /* C68 / M37450 program ROM */
	ROM_LOAD( "sas1_mpr0c.8l", 0x000000, 0x020000, CRC(21379550) SHA1(2f2b43ca526d1a77c80f81d0e1f22155d90f725d) )

	ROM_REGION(0x80000, "subcpu", 0)  /* Z84C011 program ROM */
	ROM_LOAD( "sas1_spr0.7f", 0x000000, 0x080000, CRC(3bc14ba3) SHA1(7a75281621f23107c5c3c1a09831be2f8bb93540) )

	ROM_REGION(0x2000, "at28c64", 0)        /* AT28C64 parallel EEPROM (not yet supported by MAME) */
	ROM_LOAD( "m28c64a.9l",   0x000000, 0x002000, CRC(d65d4176) SHA1(dd9b529a729685f9535ae7f060f67d75d70d9567) )

	ROM_REGION(0x40000, "oki", 0)
	ROM_LOAD( "unknown_label.5e", 0x000000, 0x040000, CRC(fa75e91e) SHA1(d06ca906135a3f23c1f0dadff75f940ea7ca0e4a) )
ROM_END

GAME( 1996,  shootaw2,  0,  m74,  m74, m74_state, empty_init, ROT0, "Namco", "Shoot Away II", MACHINE_NOT_WORKING )
