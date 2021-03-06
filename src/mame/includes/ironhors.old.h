// license:BSD-3-Clause
// copyright-holders:Mirko Buffoni, Couriersud
/*************************************************************************

    IronHorse

*************************************************************************/

#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "sound/discrete.h"

#define MASTER_CLOCK        XTAL(18'432'000)

class ironhors_state : public driver_device
{
public:
	ironhors_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_disc_ih(*this, "disc_ih"),
		m_interrupt_enable(*this, "int_enable"),
		m_scroll(*this, "scroll"),
		m_colorram(*this, "colorram"),
		m_videoram(*this, "videoram"),
		m_spriteram2(*this, "spriteram2"),
		m_spriteram(*this, "spriteram")
		{ }

	DECLARE_WRITE8_MEMBER(sh_irqtrigger_w);
	DECLARE_WRITE8_MEMBER(videoram_w);
	DECLARE_WRITE8_MEMBER(colorram_w);
	DECLARE_WRITE8_MEMBER(charbank_w);
	DECLARE_WRITE8_MEMBER(palettebank_w);
	DECLARE_WRITE8_MEMBER(flipscreen_w);
	DECLARE_WRITE8_MEMBER(filter_w);
	DECLARE_READ8_MEMBER(farwest_soundlatch_r);
	DECLARE_WRITE8_MEMBER(irq_ctrl_w);
	DECLARE_WRITE8_MEMBER(irq_enable_w);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_farwest(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	TIMER_DEVICE_CALLBACK_MEMBER(irq);
	TIMER_DEVICE_CALLBACK_MEMBER(farwest_irq);
	TIMER_DEVICE_CALLBACK_MEMBER(interrupt_tick);
	INTERRUPT_GEN_MEMBER(farwest_irq);
	INTERRUPT_GEN_MEMBER(farwest_irq_nmi);

	DECLARE_PALETTE_INIT(ironhors);
	DECLARE_VIDEO_START(farwest);

	void farwest(machine_config &config);
	void ironhors(machine_config &config);
	void farwest_master_map(address_map &map);
	void farwest_slave_map(address_map &map);
	void master_map(address_map &map);
	void slave_io_map(address_map &map);
	void slave_map(address_map &map);
protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

private:
	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;
	required_device<discrete_device> m_disc_ih;

	/* memory pointers */
	required_shared_ptr<uint8_t> m_interrupt_enable;
	required_shared_ptr<uint8_t> m_scroll;
	required_shared_ptr<uint8_t> m_colorram;
	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_spriteram2;
	required_shared_ptr<uint8_t> m_spriteram;

	/* video-related */
	tilemap_t    *m_bg_tilemap;
	int        m_palettebank;
	int        m_charbank;
	int        m_spriterambank;

	/* misc */
	uint8_t m_interrupt_mask;
	uint8_t m_interrupt_ticks;
	uint8_t m_irq_enable;
	uint8_t m_nmi_enable;

	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(farwest_get_bg_tile_info);

	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect );
	void farwest_draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect );
};
