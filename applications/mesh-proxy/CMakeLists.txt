#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#
cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mesh-proxy)

target_sources(app PRIVATE
	src/main.c
	src/model_handler.c)
target_include_directories(app PRIVATE include)
