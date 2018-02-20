// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

 konmedal68k.cpp: Konami 68000 based medal games

 Pittanko Zaurus (ピッタンコ　ザウルス)
 GS562
 (c) 1995 Konami

 Konami ICs:
 K058143 + K056832 = tilemaps
 K055555 = priority blender
 K056766 = color DAC
 K056879 = input/EEPROM interface

 800000 = control
 bit 3 = write 1 to enable or ack IRQ 3
 bit 4 = write 1 to enable or ack IRQ 4

***************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "machine/eepromser.h"
#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "sound/ymz280b.h"
#include "video/k054156_k054157_k056832.h"
#include "video/k055555.h"
#include "video/konami_helper.h"
#include "screen.h"
#include "speaker.h"

class konmedal68k_state : public driver_device
{
public:
	konmedal68k_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_k056832(*this, "k056832"),
		m_k055555(*this, "k055555"),
		m_palette(*this, "palette"),
		m_ymz(*this, "ymz")
	{ }

	uint32_t screen_update_konmedal68k(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void fill_backcolor(bitmap_ind16 &bitmap, const rectangle &cliprect, int pen_idx, int mode);

	void kzaurus(machine_config &config);

	K056832_CB_MEMBER(tile_callback);
	TIMER_DEVICE_CALLBACK_MEMBER(scanline);

	DECLARE_WRITE16_MEMBER(control_w)
	{
		m_control = data & 0xff;

		if (!(data & 0x8))
		{
			m_maincpu->set_input_line(M68K_IRQ_3, CLEAR_LINE);
		}

		if (!(data & 0x10))
		{
			m_maincpu->set_input_line(M68K_IRQ_4, CLEAR_LINE);
		}
	}
	DECLARE_WRITE16_MEMBER(control2_w) { m_control2 = data & 0xff; }

	DECLARE_READ16_MEMBER(vrom_r)
	{
		if (m_control2 & 0x10)
		{
			offset |= 0x1000;
		}

		return m_k056832->piratesh_rom_r(space, offset, mem_mask);
	}

	void kzaurus_main(address_map &map);
protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

	static constexpr int NUM_LAYERS = 4;

private:
	required_device<cpu_device> m_maincpu;
	required_device<k056832_device> m_k056832;
	required_device<k055555_device> m_k055555;
	required_device<palette_device> m_palette;
	required_device<ymz280b_device> m_ymz;

	u8 m_control, m_control2;
};

void konmedal68k_state::video_start()
{
	m_k056832->set_layer_offs(0, -4, -8);  // title on title screen
	m_k056832->set_layer_offs(1, 12, 8);    // konami logo on title screen
	m_k056832->set_layer_offs(2, 6, -8);
	m_k056832->set_layer_offs(3, 6, -8);
}

TIMER_DEVICE_CALLBACK_MEMBER(konmedal68k_state::scanline)
{
	int scanline = param;

	if ((scanline == 240) && (m_control & 0x8))
	{
		m_maincpu->set_input_line(M68K_IRQ_3, ASSERT_LINE);

	}

	if ((scanline == 255) && (m_control & 0x10))
	{
		m_maincpu->set_input_line(M68K_IRQ_4, ASSERT_LINE);
	}
}

K056832_CB_MEMBER(konmedal68k_state::tile_callback)
{
}

// modified from version in mame/video/k054338.cpp
void konmedal68k_state::fill_backcolor(bitmap_ind16 &bitmap, const rectangle &cliprect, int pen_idx, int mode)
{
	if ((mode & 0x02) == 0) // solid fill
	{
		bitmap.fill(pen_idx, cliprect);
	}
	else
	{
		uint16_t *dst_ptr = &bitmap.pix16(cliprect.min_y);
		int dst_pitch = bitmap.rowpixels();

		if ((mode & 0x01) == 0) // vertical gradient fill
		{
			pen_idx += cliprect.min_y;
			for(int y = cliprect.min_y; y <= cliprect.max_y; y++)
			{
				for(int x = cliprect.min_x; x <= cliprect.max_x; x++)
				{
					dst_ptr[x] = pen_idx;
				}

				pen_idx++;
				dst_ptr += dst_pitch;
			}
		}
		else    // horizontal gradient fill
		{
			pen_idx += cliprect.min_x;
			dst_ptr += cliprect.min_x;
			for(int y = cliprect.min_y; y<= cliprect.max_y; y++)
			{
				for(int x = cliprect.min_x; x <= cliprect.max_x; x++)
				{
					dst_ptr[x] = pen_idx;
				}
				dst_ptr += dst_pitch;
			}
		}
	}
}

uint32_t konmedal68k_state::screen_update_konmedal68k(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	static const int order[4] = { 0, 1, 3, 2 };
	int enables = m_k055555->K055555_read_register(K55_INPUT_ENABLES);

	screen.priority().fill(0, cliprect);

	fill_backcolor(bitmap, cliprect, (m_k055555->K055555_read_register(0) << 9), m_k055555->K055555_read_register(1));

	for (int i = 0; i < NUM_LAYERS; i++)
	{
		int layer = order[i];

		if (enables & (K55_INP_VRAM_A << layer))
		{
			m_k056832->tilemap_draw(screen, bitmap, cliprect, layer, 0, 1 << i);
		}
	}

	return 0;
}

ADDRESS_MAP_START(konmedal68k_state::kzaurus_main)
	AM_RANGE(0x000000, 0x07ffff) AM_ROM AM_REGION("maincpu", 0)
	AM_RANGE(0x400000, 0x403fff) AM_RAM
	AM_RANGE(0x800000, 0x800001) AM_WRITE(control_w)
	AM_RANGE(0x800004, 0x800005) AM_READ_PORT("DSW")
	AM_RANGE(0x800006, 0x800007) AM_READ_PORT("IN1")
	AM_RANGE(0x800008, 0x800009) AM_READ_PORT("IN0")
	AM_RANGE(0x810000, 0x810001) AM_WRITE(control2_w)
	AM_RANGE(0x830000, 0x83003f) AM_DEVREADWRITE("k056832", k056832_device, word_r, word_w)
	AM_RANGE(0x840000, 0x84000f) AM_DEVWRITE("k056832", k056832_device, b_word_w)
	AM_RANGE(0x85001c, 0x85001f) AM_WRITENOP
	AM_RANGE(0x870000, 0x87005f) AM_DEVWRITE("k055555", k055555_device, K055555_word_w)
	AM_RANGE(0x880000, 0x880003) AM_DEVREADWRITE8("ymz", ymz280b_device, read, write, 0xff00)
	AM_RANGE(0xa00000, 0xa01fff) AM_DEVREADWRITE("k056832", k056832_device, ram_word_r, ram_word_w)
	AM_RANGE(0xa02000, 0xa03fff) AM_DEVREADWRITE("k056832", k056832_device, ram_word_r, ram_word_w)
	AM_RANGE(0xb00000, 0xb01fff) AM_RAM_DEVWRITE("palette", palette_device, write16) AM_SHARE("palette")
	AM_RANGE(0xc00000, 0xc01fff) AM_READ(vrom_r)
ADDRESS_MAP_END

static INPUT_PORTS_START( kzaurus )
	PORT_START("IN0")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Test") PORT_CODE(KEYCODE_F2)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0xff1f, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )  // medal ack
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN3 )    // medal
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0xf0ff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x07, 0x00, "Coin Slot 1" )   PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, "5 Coins/2 Credits" )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x78, 0x00, "Coin Slot 2" )   PORT_DIPLOCATION("SW1:4,5,6,7")
	PORT_DIPSETTING(    0x00, "16 Medals" )
	PORT_DIPSETTING(    0x08, "15 Medals" )
	PORT_DIPSETTING(    0x10, "14 Medals" )
	PORT_DIPSETTING(    0x18, "13 Medals" )
	PORT_DIPSETTING(    0x20, "12 Medals" )
	PORT_DIPSETTING(    0x28, "11 Medals" )
	PORT_DIPSETTING(    0x30, "10 Medals" )
	PORT_DIPSETTING(    0x38, "9 Medals" )
	PORT_DIPSETTING(    0x40, "8 Medals" )
	PORT_DIPSETTING(    0x48, "7 Medals" )
	PORT_DIPSETTING(    0x50, "6 Medals" )
	PORT_DIPSETTING(    0x58, "5 Medals" )
	PORT_DIPSETTING(    0x60, "4 Medals" )
	PORT_DIPSETTING(    0x68, "3 Medals" )
	PORT_DIPSETTING(    0x70, "2 Medals" )
	// PORT_DIPSETTING(    0x78, "2 Medals" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )   PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )

	PORT_DIPNAME( 0x0f00, 0x0000, "Standard of Payout" ) PORT_DIPLOCATION("SW2:1,2,3,4")
	PORT_DIPSETTING(    0x0000, "90%" )
	PORT_DIPSETTING(    0x0100, "85%" )
	PORT_DIPSETTING(    0x0200, "80%" )
	PORT_DIPSETTING(    0x0300, "75%" )
	PORT_DIPSETTING(    0x0400, "70%" )
	PORT_DIPSETTING(    0x0500, "65%" )
	PORT_DIPSETTING(    0x0600, "60%" )
	PORT_DIPSETTING(    0x0700, "55%" )
	PORT_DIPSETTING(    0x0800, "50%" )
	PORT_DIPSETTING(    0x0900, "45%" )
	PORT_DIPSETTING(    0x0a00, "40%" )
	PORT_DIPSETTING(    0x0b00, "35%" )
	PORT_DIPSETTING(    0x0c00, "30%" )
	PORT_DIPSETTING(    0x0d00, "25%" )
	PORT_DIPSETTING(    0x0e00, "20%" )
	PORT_DIPSETTING(    0x0f00, "15%" )
	PORT_DIPNAME( 0x3000, 0x0000, "Play Timer" )         PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x0000, "30 sec" )
	PORT_DIPSETTING(    0x1000, "24 sec" )
	PORT_DIPSETTING(    0x2000, "18 sec" )
	PORT_DIPSETTING(    0x3000, "12 sec" )
	PORT_DIPNAME( 0x4000, 0x0000, "Backup Memory" )      PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x4000, "Keep" )
	PORT_DIPSETTING(    0x0000, "Clear" )
	PORT_DIPNAME( 0x8000, 0x0000, "Demo Sound" )         PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
INPUT_PORTS_END

void konmedal68k_state::machine_start()
{
}

void konmedal68k_state::machine_reset()
{
	m_control = m_control2 = 0;
}

MACHINE_CONFIG_START(konmedal68k_state::kzaurus)
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, XTAL(33'868'800)/4 )    // 33.8688 MHz crystal verified on PCB
	MCFG_CPU_PROGRAM_MAP(kzaurus_main)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", konmedal68k_state, scanline, "screen", 0, 1)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(59.62)  /* verified on pcb */
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(40, 400-1, 16, 240-1)
	MCFG_SCREEN_UPDATE_DRIVER(konmedal68k_state, screen_update_konmedal68k)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 8192)
	MCFG_PALETTE_ENABLE_SHADOWS()
	MCFG_PALETTE_FORMAT(XBGR)

	MCFG_DEVICE_ADD("k056832", K056832, 0)
	MCFG_K056832_CB(konmedal68k_state, tile_callback)
	MCFG_K056832_CONFIG("gfx1", K056832_BPP_4dj, 1, 0, "none")
	MCFG_K056832_PALETTE("palette")

	MCFG_K055555_ADD("k055555")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_DEVICE_ADD("ymz", YMZ280B, XTAL(33'868'800)/2) // 33.8688 MHz xtal verified on PCB
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END

ROM_START( kzaurus )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* main program */
	ROM_LOAD16_WORD_SWAP( "540-b05-2n.bin", 0x000000, 0x080000, CRC(110d4ecb) SHA1(8903783f62ad5a983242a0fe8d835857964abc43) )

	ROM_REGION( 0x100000, "gfx1", 0 )   /* tilemaps */
	ROM_LOAD( "540-a06-14n.bin", 0x000000, 0x080000, CRC(260ad79e) SHA1(fb56bf6e59e78b2bd1f8df17c9c8fd0d1700dced) )
	ROM_LOAD( "540-a07-17n.bin", 0x080000, 0x080000, CRC(442bcec2) SHA1(3100de8c146a28284ae3ab8763e5b1c6fb1755c2) )

	ROM_REGION( 0x80000, "ymz", 0 )
	ROM_LOAD( "540-a01-2f.bin", 0x000000, 0x080000, CRC(391c6ee6) SHA1(a345934687a8abf818350d0597843a1159395fc0) )
ROM_END

GAME( 1995, kzaurus,    0, kzaurus, kzaurus,  konmedal68k_state, 0, ROT0, "Konami", "Pittanko Zaurus", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
