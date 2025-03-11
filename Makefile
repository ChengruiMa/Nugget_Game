# Updated Makefile

# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g
LDFLAGS = -lncurses

# Include paths – adjust if your directory structure differs
INC = -I. -I../support -I../common

# Production source files
SRCS_SERVER = server.c map.c visibility.c player.c message.c
SRCS_CLIENT = client.c client_state.c display.c network.c map.c visibility.c player.c

# Production executables
NAME_SERVER = server
NAME_CLIENT = client

# Test source files
TEST_GRID          = test_grid.c map.c visibility.c player.c
TEST_PLAYER        = test_player.c map.c player.c
TEST_VISIBILITY    = test_visibility.c map.c visibility.c
TEST_CLIENT_STATE  = test_client_state.c client_state.c map.c
TEST_NETWORK       = test_network.c client_state.c network.c map.c display.c
TEST_DISPLAY       = test_display.c client_state.c display.c map.c
TEST_SERVER        = test_server.c server.c map.c visibility.c player.c message.c

# Test executables list
TEST_EXES = test_grid test_player test_visibility test_client_state test_network test_display test_server

# Default target: build production executables
all: $(NAME_SERVER) $(NAME_CLIENT)

# Production build rules
$(NAME_SERVER): $(SRCS_SERVER:.c=.o)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

$(NAME_CLIENT): $(SRCS_CLIENT:.c=.o)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

# Generic rule for object files
%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Test targets
test_grid: $(TEST_GRID)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_GRID)

test_player: $(TEST_PLAYER)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_PLAYER)

test_visibility: $(TEST_VISIBILITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_VISIBILITY)

test_client_state: $(TEST_CLIENT_STATE)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_CLIENT_STATE)

test_network: $(TEST_NETWORK)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_NETWORK) $(LDFLAGS)

test_display: $(TEST_DISPLAY)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_DISPLAY) $(LDFLAGS)

test_server: $(TEST_SERVER)
	$(CC) $(CFLAGS) $(INC) -o $@ $(TEST_SERVER)

# Integration test target – runs the provided integration test script
integration: $(NAME_SERVER) $(NAME_CLIENT)
	./run_integration_tests.sh

# Clean rules
clean:
	rm -f *.o $(NAME_SERVER) $(NAME_CLIENT) $(TEST_EXES)

fclean: clean
	rm -f $(NAME_SERVER) $(NAME_CLIENT) $(TEST_EXES)

re: fclean all

.PHONY: all clean fclean re integration $(TEST_EXES)
