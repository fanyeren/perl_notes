# Description: Makefile
#
# Copyright (c) 2008 by Baidu.com, Incorporation, P.R.C
# All rights reserved.
#
#

# Parameter
TARGET = zkctl.bin
#TOOLS = check_merge
TOOLS = zksh util
CONF_NAME = zkctl.conf


VERSION = 1.0.1.0
CVS_PATH = ps/se/aop/zkctl
COVFILE = ./test.cov

LIB2_BASE := ../../../../lib2-64/
LIBPUB_BASE := ../../../../public/
THIRD_BASE := ../../../../third-64/
LIBSE_BASE := ../../../se/
LIBLASER_BASE := ../../../laser/

DEPENDS := deps
ifneq ($(findstring clean, $(MAKECMDGOALS)), )
	DEPENDS :=
endif

# ---------- Source files -----------
SRCS := version.cpp
#SRCS := util.cpp zkctl_index.cpp zk_log.cpp version.cpp

TSRCS := $(TOOLS:=.cpp)

# ------------- Sub Dirs ------------- 
# 需要预先编译的子目录
#
SUBDIRS :=

# Lib2, lib2-64;
# 所需要使用的库的目录名, lib2下, 并且发布成lib和include两个子目录
#
LIB2 := ullib bsl
DLIB2 := $(addprefix $(LIB2_BASE), $(LIB2))

# Lib2, lib2-64;
# 所需要使用的库的目录名, lib2下, 但是没有发布成lib和include子目录
#
LIB2_EX := 
DLIB2_EX := $(addprefix $(LIB2_BASE), $(LIB2_EX))

# Lib public
# public下的lib，发布为include和lib子目录
#
LIBPUB2 := configure mcpack spreg bslext idlcompiler comlog-plugin
DLIBPUB2 := $(addprefix $(LIBPUB_BASE), $(LIBPUB2))

# Lib public
# public下的lib, 但是没有发布万lib和include子目录
#
LIBPUB := connectpool uconv/output
DLIBPUB := $(addprefix $(LIBPUB_BASE), $(LIBPUB))

# Lib Third
# third下的lib
#
LIB3 := zookeeper pcre
DLIB3 := $(addprefix $(THIRD_BASE), $(LIB3))

# Se Lib
# 发布的依赖库目录名, ps/se/ 下, 发布成lib和include子目录
#
LIB_SE := aop/libzkwrapper
DLIB_SE := $(addprefix $(LIBSE_BASE), $(LIB_SE))

# Se Lib
# 非发布的依赖库目录名, ps/se/ 下, 没有发布成lib和include子目录
#
LIB_SE_EX := 
DLIB_SE_EX := $(addprefix $(LIBSE_BASE), $(LIB_SE_EX))

# Laser Lib
# 非发布的依赖库目录名, ps/laser/ 下, 没有发布成lib和include子目录
#
LIB_LASER := 
DLIB_LASER := $(addprefix $(LIBLASER_BASE), $(LIB_LASER))

# Lib others
# 其他库
DLIBOTHER := com/btest/gtest

# Lib names
# 依赖的库的名字
#
LIBS := zkwrapper zookeeper_mt pcre pcrecpp pcreposix mcpack_idl connectpool comlog \
	bslext bsl_var_serializer bsl_bml bsl_var_scripting bsl_vs \
	uconv config bsl bsl_utils bsl_var bsl_var_implement bsl_var_utils ullib spreg \
	ssl pthread 
	#galileo zookeeper_mt config bslext bsl spreg uldict ullib tcmalloc 

# Head files
#
HEADS := $(wildcard *.h)

# Object files
#
OBJS := $(SRCS:.cpp=.o)

#===============================================================================

# C++ compiler options
#
CXX := g++
AR := ar
RM := rm -rf

# Extra flags to give to the C preprocessor and programs that use it
#
DEFINES := -D__VERSION_ID__="\"$(VERSION)\"" -D__CVSPATH__="\"$(CVS_PATH)\"" \
	-D__BUILD_ENV__="\"$(USER)@$(HOSTNAME)\""

INCLUDES := -I. $(addprefix -I, $(DLIBOTHER)) \
	$(addprefix -I, $(DLIB_LASER)) \
	$(addprefix -I, $(DLIB_SE_EX)) \
	$(addprefix -I, $(DLIBPUB)) \
	$(addprefix -I, $(DLIB2_EX)) \
	$(addprefix -I, $(addsuffix /output/include/, $(DLIBPUB2))) \
	$(addprefix -I, $(addsuffix /output/include/, $(DLIB_SE))) \
	$(addprefix -I, $(addsuffix /include/, $(DLIB2))) \
	$(addprefix -I, $(addsuffix /include/, $(DLIB3))) \
	-I$(LIBSE_BASE)/include \
	-I../include

CPPFLAGS := $(DEFINES) $(INCLUDES)


# Extra flags to give to the C++ compiler
#
CXXFLAGS := -g -Wall -Wextra -pipe -O0 \
	-D_CPP_UNIT

# Extra flags to give to compilers when they are supposed to invoke the linker
#
LDFLAGS := -g 
LIBDIRS := . $(DLIBOTHER) $(DLIB_LASER) $(DLIB_SE_EX) $(DLIBPUB) \
	$(addsuffix /output/lib/, $(DLIBPUB2)) \
	$(addsuffix /output/lib/, $(DLIB_SE)) \
	$(DLIB2_EX) $(addsuffix /lib/, $(DLIB2) $(DLIB3))
LDFLAGS += $(addprefix -L, $(LIBDIRS)) $(addprefix -l, $(LIBS))


# Lib Depends
# 根据 LDFLAGS, 获取依赖的非系统库实际完整路径
# 当检测到这些库更新后重新编译 version.cpp 和可执行文件
# 
D_LIBDEPS := $(patsubst -L%, %, $(filter -L%, $(LDFLAGS)))
USED_LIBS := $(patsubst -l%, %, $(filter -l%, $(LDFLAGS)))
VLIBDEPS := $(strip $(foreach path, $(D_LIBDEPS), \
		$(foreach name, $(USED_LIBS), \
			$(shell \
			[ -f $(path)/lib$(name).a ] \
			&& echo $(path)/lib$(name).a))))


# 默认编译规则
#.cpp.o :
#	$(CXX) $< -c $(CXXFLAGS) $(CPPFLAGS)
#	@echo

# 工具链接规则
define TOOLS_template
$(1) : $(1).o $(OBJS)
	$$(CXX) -o $$@ $$^ $$(LDFLAGS)
	@echo
endef

#===============================================================================

all : $(DEPENDS) $(TARGET) $(TOOLS) .publish

# Make target
$(TARGET) : $(OBJS) zkctl.o
	$(CXX) -o $@ $^ $(LDFLAGS)


# Make tools
# 根据模板生成工具编译规则
#
$(foreach prog,$(TOOLS),$(eval $(call TOOLS_template,$(prog))))


# Publish
.publish : output

output : $(TARGET) $(CONF_NAME) $(TOOLS) scripts/*
	@if [ -d output ]; then rm -rf output; fi
	@if [ -d zkctl_impl ]; then rm -fr zkctl_impl; fi
	@if [ -d zkctl_rpc_scripts ]; then rm -fr zkctl_rpc_scripts; fi
	mkdir zkctl_impl
	cp $(TARGET) $(CONF_NAME) $(TOOLS) zkctl_impl
	find scripts/ -maxdepth 1 -type f -exec cp "{}" zkctl_impl/ \;
	mkdir zkctl_rpc_scripts
	find rpc_scripts/ -maxdepth 1 -type f -exec cp "{}" zkctl_rpc_scripts/ \;
	mkdir output
	cp zkctl output
	cp -r zkctl_impl output
	cp -r zkctl_rpc_scripts output

# CCover 编译 ccover 版本
# 编译前先设置环境变量 export COVFILE=`pwd`/${module_name}.cov 默认为./test.cov
# 设置环境变量可以将多个模块的cov文件合并
# 输出结果${COVFILE}最终放到output中
# ccover编译后，再普通gcc编译会出错，因此ccover编译完很后运行clean清理环境
#
ccover : distclean .init_ccov all .finish_ccov
	@$(MAKE) -C ./ clean
	mv $(COVFILE) output/
	@echo

.init_ccov :
	@which cov01 1>/dev/null 2>/dev/null || (echo "cov01 not found"; exit 1)
	@cov01 -1 --no-banner || (echo "Enable Coverage Build failed."; exit 1)

.finish_ccov :
	@cov01 -0 --no-banner || (echo "Disable Coverage Build failed."; exit 1)

# Depends
# 生成依赖关系
#
deps : $(SRCS) $(TSRCS) $(HEADS) Makefile
	@$(CXX) -MM -MG $(CXXFLAGS) $(CPPFLAGS) $(filter %.c %.cpp, $^) > $@

deps_d : deps
	@sed 's/\($*\)\.o[ :]*/\1_d.o : /g' deps > deps_d

# Clean
clean :
	$(RM) $(TARGET) $(TARGET_D) $(TOOLS) deps deps_d output *.o *~ \
		.\#*.cpp* .\#*.h* .\#Makefile.*
	$(RM) zkctl_impl
	@echo

# Clean all
distclean : clean
	@echo

# Clean and make all
rebuild : clean all

# Make debug version (tools下 不区分debug和release)
debug : all

# Make release version (tools下 不区分debug和release)
release : all

test: $(MODULE_NAME)
	@cd scripts; python placement-plan_generate -t
	@cd scripts; python test_placement_plan_generate.py
	@sh test.sh

# 打印环境变量，用于检查Makefile生成的规则是否正确
# check_debug 为检查debug版本的情况
#
check_debug : check
check :
	@echo -e "TARGET = $(TARGET)\n"
	@echo -e "CPPFLAGS = $(CPPFLAGS)\n"
	@echo -e "CXXFLAGS = $(CXXFLAGS)\n"
	@echo -e "LDFLAGS = $(LDFLAGS)\n"
	@echo -e "COVFILE = $(COVFILE)\n"

.PHONY : clean distclean all rebuild release debug tags .publish ccover \
	check check_debug test

# Depends
# version.cpp依赖. 凡是有源文件，头文件以及依赖的lib文件更新
# 都会重新编译version.o和可执行文件，从而更新 -v所看到的编译时间和版本号
#
version.cpp : Makefile $(filter-out version.cpp, $(SRCS)) $(HEADS) $(VLIBDEPS)
	@touch version.cpp

ifneq (,$(DEPENDS))
-include $(DEPENDS)
endif


