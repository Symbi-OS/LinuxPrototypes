REDIS_CMD=redis-server --protected-mode no --save '' --appendonly no
ALL_LEAKS= --leak-check=full --show-leak-kinds=all
FD_LEAKS= --track-fds=yes
run:
	$(REDIS_CMD)

run_valgrind_all:
	valgrind $(ALL_LEAKS) $(FD_LEAKS)  \
		$(REDIS_CMD)

run_valgrind_fd:
	valgrind $(FD_LEAKS) \
		$(REDIS_CMD)

generate_valgrind_suppressions:
	valgrind $(ALL_LEAKS) --suppressions=suppressions.supp --log-file=valgrind_output.txt \
		$(REDIS_CMD) > working_binary_valgrind_output.txt 2>&1

f_generate_valgrind_suppressions:
	@echo "Generating valgrind suppressions"
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=suppressions.supp \
	redis-server --protected-mode no --save '' --appendonly no > /dev/null 2>&1

x_generate_valgrind_suppressions:
	@echo "Generating valgrind suppressions"
	@valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind_output.txt \
	redis-server --protected-mode no --save '' --appendonly no > /dev/null 2>&1

z_generate_valgrind_suppressions:
	@echo "Generating valgrind suppressions"
	@echo "Redis will start more slowly than normal under valgrind."
	@echo "Don't expect redis output"
	valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=all --log-file=working_binary_valgrind_output.txt \
	redis-server --protected-mode no --save '' --appendonly no > /dev/null 2>&1

filter:
	grep -vE '^==' working_binary_valgrind_output.txt > working_binary_valgrind_output_filtered.txt
	grep -E '^{.*}$$' working_binary_valgrind_output_filtered.txt > generated_suppressions.supp
	awk '/<insert_a_suppression_name_here>/ {sub("<insert_a_suppression_name_here>", "suppression_" ++i)} 1' generated_suppressions.supp > generated_suppressions_numbered.supp
	mv generated_suppressions_numbered.supp generated_suppressions.supp

# valgrind $(ALL_LEAKS) --gen-suppressions=all \
# 	$(REDIS_CMD) > working_binary_valgrind_output.txt 2>&1
# redis-server --protected-mode no --save "" --appendonly no > /dev/null 2>&1


isolate_valgrind_suppressions:
	

run_valgrind_with_suppressions:
	valgrind $(ALL_LEAKS) --suppressions=auto_generated.supp \
		$(REDIS_CMD)

get_pid:
	ps -ef | grep redis-server

# Makefile

input_file := working_binary_valgrind_output.txt
output_file := auto_generated.supp

# extract-suppressions:
# 	@echo "Extracting suppressions from $(input_file) to $(output_file)"
# 	@grep -A13 -E "{\s*$$" $(input_file) | sed -e '/--/d' > $(output_file)

extract-suppressions:
	@echo "Extracting suppressions from $(input_file) to $(output_file)"
	@awk '/^{$$/{flag=1;cnt=0}/^}$$/{flag=0}flag{if(cnt>0){print};cnt++}' $(input_file) > $(output_file)

clean:
	rm -f auto_generated.supp working_binary_valgrind_output.txt