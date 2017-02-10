default: master_controller.cpp
	@g++ master_controller.cpp -o controller

debug: master_controller.c
	@g++ -g master_controller.cpp -o debug

created = mastercontroller debug
clean:
	@$(RM) $(created)