#!/bin/bash
# Ensure the script is executable: chmod +x tests/run_integration_tests.sh

# Build the server and client
make all

# Start the server in the background using the integration test map
./server -m ../maps/test_integration.txt -s 1234 &
SERVER_PID=$!
echo "Server started with PID $SERVER_PID"
sleep 1  # Allow time for initialization

# Run the client with simulated input from client_input.txt
./client localhost 1234 test_player < client_input.txt &
CLIENT_PID=$!
echo "Client started with PID $CLIENT_PID"

# Let the test run for 5 seconds
sleep 5

# Terminate client and server
kill -SIGINT $CLIENT_PID
kill -SIGINT $SERVER_PID

echo "Integration test complete."

