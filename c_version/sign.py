#!/usr/bin/python3
import sys

boot_block_filename = sys.argv[1]

with open(boot_block_filename, 'rb') as file:
    data = bytearray(file.read())

bytes_read = len(data)

if bytes_read > 510:
    print("boot block is too large: {} bytes (max 510)".format(bytes_read))
    sys.exit(1)

print("boot block is {} bytes (max 510)".format(bytes_read))

# Append the boot block signature at bytes 511 and 512
data += bytearray(b'\x00') * (510 - bytes_read) + bytearray(b'\x55\xAA')
with open(boot_block_filename, 'wb') as file:
    file.write(data)
