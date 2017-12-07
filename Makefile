EXEC = raytracing
EXEC_M_U = raytracing_m_u
EXEC_SSE = raytracing_sse
EXEC_AVX = raytracing_avx
GIT_HOOKS := .git/hooks/pre-commit
.PHONY: all
all: $(GIT_HOOKS) $(EXEC) $(EXEC_M_U) $(EXEC_SSE) $(EXEC_AVX)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

CC ?= gcc
CFLAGS = \
	-std=gnu99 -Wall -O0 -g 
LDFLAGS = \
	-lm 

ifeq ($(strip $(PROFILE)),1)
PROF_FLAGS = -pg
CFLAGS += $(PROF_FLAGS)
LDFLAGS += $(PROF_FLAGS) 
endif

OBJS := \
	objects.o \
	raytracing.o \
	main.o
OBJS_M_U := \
        objects_m_u.o \
        raytracing_m_u.o \
        main_m_u.o
OBJS_SSE := \
        objects_sse.o \
        raytracing_sse.o \
        main_sse.o
OBJS_AVX := \
	objects_avx.o \
        raytracing_avx.o \
        main_avx.o 
%.o: %.c
	$(CC) $(CFLAGS) -c -DORG -o $@ $<

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%_m_u.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC_M_U): $(OBJS_M_U)
	$(CC) -o $@ $^ $(LDFLAGS)
%_sse.o: %.c
	$(CC) $(CFLAGS) -c -DSSE -o $@ $<

$(EXEC_SSE): $(OBJS_SSE)
	$(CC) -o $@ $^ $(LDFLAGS)

%_avx.o: %.c
	$(CC) $(CFLAGS) -c -DAVX -mavx -o $@ $<
$(EXEC_AVX): $(OBJS_AVX)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: use-models.h
use-models.h: models.inc Makefile
	@echo '#include "models.inc"' > use-models.h
	@egrep "^(light|sphere|rectangular) " models.inc | \
	    sed -e 's/^light /append_light/g' \
	        -e 's/light[0-9]/(\&&, \&lights);/g' \
	        -e 's/^sphere /append_sphere/g' \
	        -e 's/sphere[0-9]/(\&&, \&spheres);/g' \
	        -e 's/^rectangular /append_rectangular/g' \
	        -e 's/rectangular[0-9]/(\&&, \&rectangulars);/g' \
	        -e 's/ = {//g' >> use-models.h
cache-test: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches;
	perf stat --repeat 10 \
                -e cache-misses,cache-references,instructions,cycles \
                sudo chrt -f 99 taskset -c 0 ./raytracing;
	echo 3 | sudo tee /proc/sys/vm/drop_caches;
	perf stat --repeat 10 \
                -e cache-misses,cache-references,instructions,cycles \
                sudo chrt -f 99 taskset -c 0 ./raytracing_m_u;
	echo 3 | sudo tee /proc/sys/vm/drop_caches;
	perf stat --repeat 10 \
                -e cache-misses,cache-references,instructions,cycles \
                sudo chrt -f 99 taskset -c 0 ./raytracing_sse;
	perf stat --repeat 10 \
                -e cache-misses,cache-references,instructions,cycles \
                sudo chrt -f 99 taskset -c 0 ./raytracing_avx
check: $(EXEC)
	@./$(EXEC) && diff -u baseline.ppm out.ppm || (echo Fail; exit)
	@echo "Verified OK"
check_m_u: $(EXEC_M_U)
	@./$(EXEC_M_U) && diff -u baseline.ppm out.ppm || (echo Fail; exit)
	@echo "Verified OK"
check_sse: $(EXEC_SSE)
	@./$(EXEC_SSE) && diff -u baseline.ppm out.ppm || (echo Fail; exit)
	@echo "Verified OK"
check_avx: $(EXEC_AVX)
	@./$(EXEC_AVX) && diff -u baseline.ppm out.ppm || (echo Fail; exit)
	@echo "Verified OK"
clean:
	$(RM) $(EXEC_M_U) $(AVX) $(OBJS_AVX) $(SSE) $(EXEC) $(OBJS_M_U) $(OBJS_SSE) $(OBJS) use-models.h \
		out.ppm gmon.out
