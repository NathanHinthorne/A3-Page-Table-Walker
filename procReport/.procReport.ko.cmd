cmd_/home/ubuntu/shared/procReport/procReport.ko := ld -r  -EL  -maarch64elf -z noexecstack   --build-id  -T ./scripts/module-common.lds -T ./arch/arm64/kernel/module.lds -o /home/ubuntu/shared/procReport/procReport.ko /home/ubuntu/shared/procReport/procReport.o /home/ubuntu/shared/procReport/procReport.mod.o;  true
