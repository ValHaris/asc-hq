MAINTAINERCLEANFILES    = Makefile.in asc.6 asc_bin2text.6 asc_demount.6 \
asc_mount.6 asc_mapedit.6 asc_makegfx.6 asc_viewid.6 asc_map2pcx.6 \
asc_weaponguide.6

man_MANS		= asc.6 asc_bin2text.6 asc_demount.6 asc_mount.6 \
asc_mapedit.6 asc_makegfx.6 asc_viewid.6 asc_map2pcx.6 asc_weaponguide.6

## gfx2pcx, mntgraph, tank2pcx are outdated

%.6: %.sgml
	@DOCBOOK2MAN@ $< > $@

