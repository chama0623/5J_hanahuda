OBJS = setup_client.o setup_server.o chop_newline.o setup_mserver.o 
AR   =  ar -qc

mylib.a:      $(OBJS)
		$(RM) $@
		$(AR) $@  $(OBJS)
		

$(OBJS):     mylib.h

clean:
		$(RM) *.o mylib.a
