.NOTPARALLEL:

PKG_LEVEL = ..
include $(PKG_LEVEL)/Makefile.pkg.common

#https://www.sqlite.org/2017/sqlite-autoconf-3180000.tar.gz
PkgBuildDir = $(DATuner_PKG_BUILD)/sqlite
PkgTarBall = sqlite-autoconf-3180000.tar.gz
sqliteBuildDir = $(PkgBuildDir)/build
sqliteSrcDir = $(PkgBuildDir)/sqlite-autoconf-3180000

## Rules
all:: download-sqlite config-sqlite make-sqlite

#=== Build sqlite ===#

download-sqlite: $(PkgBuildDir)/$(PkgTarBall) 

$(PkgBuildDir)/$(PkgTarBall): $(PkgBuildDir)/.dir
	cd $(PkgBuildDir); \
		wget https://www.sqlite.org/2017/sqlite-autoconf-3180000.tar.gz;
	@touch $@

config-sqlite: $(sqliteBuildDir)/config.status 

$(sqliteBuildDir)/config.status: $(PkgBuildDir)/.unpacked $(sqliteBuildDir)/.dir $(sqliteSrcDir)/configure
	cd $(sqliteBuildDir); \
	$(sqliteSrcDir)/configure --prefix=$(DATuner_PKG_INSTALL); 

make-sqlite:: $(sqliteBuildDir)/sqlite

$(sqliteBuildDir)/sqlite:
	cd $(sqliteBuildDir); \
	make; \
	make install;
