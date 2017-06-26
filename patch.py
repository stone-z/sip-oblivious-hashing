import binascii
import subprocess

def convert_to_little_endian(input):
  idx = len(input)
  if idx % 2 is 1:
      input = '0' + input
      idx = idx + 1
  little_end_output = ''
  while idx is not -2:
     little_end_output += input[idx - 2 : idx]
     idx -= 2
  return little_end_output

done = False

while done is False:
   p = subprocess.Popen(['./a.out'], stdout = subprocess.PIPE)
   out, err = p.communicate()
   if(out.find('Program corrupted! Exiting!') is -1):
      done = True
      break
   lines = out.split('\n')
   failed_assert = lines[len(lines) - 3]
   address = failed_assert[failed_assert.find('0x') + 2 : failed_assert.find('>')]
   little_end_addr = convert_to_little_endian(address)
   current_val = failed_assert[failed_assert.find('Current: <') + 10: failed_assert.rfind('>')]
   little_end_val = convert_to_little_endian(current_val)
   file = open('a.out', 'r+b')
   hexdata = binascii.hexlify(file.read())
   file.close()
   padding_len = 8 - len(little_end_addr)
   value_to_replace = little_end_addr + '0' * padding_len + 'beefbeadde'
   padding_len = 8 - len(little_end_val)
   replacement = little_end_addr + '00be' + little_end_val + '0' * padding_len
   replaced = hexdata.replace(value_to_replace, replacement)
   output = binascii.unhexlify(replaced)
   output_file = open('a.out', 'wb')
   output_file.write(output)
   output_file.close()

