
installer: 
	makensis asc.nsi

tarball:
	echo "ASC version is: $(asc_version) "
	echo "top src dir is: $(abs_top_srcdir) "
	echo /tmp/ASC-$(asc_version)
#	test ! -e /tmp/ASC-$(asc_version)
	ln -s $(abs_top_srcdir) /tmp/ASC-$(asc_version)
	tar chf $(abs_top_builddir)/installer/asc-source-$(asc_version).tar --exclude-from=$(abs_top_builddir)/installer/exclude_file_list --directory=/tmp ASC-$(asc_version)/
	tar rhf $(abs_top_builddir)/installer/asc-source-$(asc_version).tar --directory=/tmp ASC-$(asc_version)/source/libs/bzlib/Makefile
	bzip2 -f $(abs_top_builddir)/installer/asc-source-$(asc_version).tar
	rm /tmp/ASC-$(asc_version)


pbpinstaller: 
	cd $(top_srcdir)/installer && makensis pbp.nsi
