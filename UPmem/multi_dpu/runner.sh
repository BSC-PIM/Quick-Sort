
source ~/lab/CAfPP/UPmem/upmem-2023.2.0-Linux-x86_64/upmem_env.sh simulator

dpu-upmem-dpurte-clang dpu.c -o dpu

# shellcheck disable=SC2006
gcc -O3 --std=c99 -o host host.c  -g `dpu-pkg-config --cflags --libs dpu`

./host host

rm -f dpu
rm -f host