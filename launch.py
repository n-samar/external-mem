#!/usr/bin/env python3

import os

list_log_element_count = [23, 24, 25, 26, 27]
list_log_mem_size = [27]
list_segment_width = [0.1, 1, 10]
list_generate_data = [1]
list_fraction_vertical = [0.01, 0.05, 0.5, 0.95, 0.99]
list_use_external = [0, 1]

os.system("mkdir data/")
for log_element_count in list_log_element_count:
    for log_mem_size in list_log_mem_size:
        for generate_data in list_generate_data:
            for segment_width in list_segment_width:
                for fraction_vertical in list_fraction_vertical:
                    for use_external in list_use_external:
                        iostat_cmd = f"iostat -g ALL -x -t 1 | grep ALL > data/iostat_main_{log_element_count}_{log_mem_size}_{segment_width}_{generate_data}_{use_external}_{fraction_vertical}.txt &"
                        os.system(iostat_cmd)
                        cmd = f"cgexec -g memory:6850 -- ./main {log_element_count} {log_mem_size} {segment_width} {generate_data} {use_external} {fraction_vertical}"
                        print(f"START: {cmd}")
                        os.system(cmd)
                        os.system("ps aux | grep root | grep iostat | awk '{print $2}' | xargs -I % kill -9 %")
