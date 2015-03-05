include ../../.config

build: build_ko build_bin

build_static: build_ko build_bin

build_dynamic: build_ko bulid_bin

build_ko:
	#@make -C ../../kernel/$(CONFIG_KERNEL_VERSION) SUBDIRS=`pwd`/source ARCH=$(CONFIG_ARCH) CROSS_COMPILE=$(CONFIG_CROSS_COMPILE) KERNEL_PATH=../../kernel/$(CONFIG_KERNEL_VERSION) modules
	@make -C /home/yanwzh/workspace/goxceed/git-driver/v1.9-dev/linux-2.6.27.55 SUBDIRS=`pwd`/source ARCH=$(CONFIG_ARCH) CROSS_COMPILE=$(CONFIG_CROSS_COMPILE) KERNEL_PATH=/home/yanwzh/workspace/goxceed/git-driver/v1.9-dev/linux-2.6.27.55 modules

build_bin:
	@make -C test 

install: copy_ko

install_static: copy_ko

install_dynamic:copy_ko

copy_ko:
	@cp source/*.ko $(INSTALL_DIR)/lib/modules/$(CONFIG_KERNEL_VERSION)
	@cp S* $(INSTALL_DIR)/home/root/etc/rcS.d
	@mkdir -p $(INSTALL_DIR)/usr/sbin
	@cp test/otp_test $(INSTALL_DIR)/usr/sbin
	@$(CONFIG_CROSS_COMPILE)strip $(INSTALL_DIR)/usr/sbin/otp_test

clean:
	#@@make -C ../../kernel/$(CONFIG_KERNEL_VERSION) SUBDIRS=`pwd`/source CONFIG_CROSS_COMPILE=$(CONFIG_CROSS_COMPILE) clean
	@@make -C /home/yanwzh/workspace/goxceed/git-driver/v1.9-dev/linux-2.6.27.55 SUBDIRS=`pwd`/source CONFIG_CROSS_COMPILE=$(CONFIG_CROSS_COMPILE) clean
	@make -C test clean
	@@rm -f source/*.order

