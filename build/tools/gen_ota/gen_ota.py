import os, sys
import subprocess

exe_path = '../otaImage/image_gen_header'
#arguments = ["-d", "COMBO", "-b", "REMAPPING", "-r"]
arguments = ["-d", "COMBO", "-b", "REMAPPING", "-r"]
py_version = sys.version_info.major

def fill(pathin, partition_size):
    path_tmp = pathin.rsplit('.', 1)
    fill_path = path_tmp[0] + '_fill.bin'
    crc_test_path = path_tmp[0] + '_ota_t_crc.bin'
    len_test_path = path_tmp[0] + '_ota_t_len.bin'
    ota_path = path_tmp[0] + '_ota.bin'
    old_chip_path = path_tmp[0] + '_ota_old_chip.bin'

    print(partition_size)
    if sys.version_info.major == 2:
        result = subprocess.call([exe_path, pathin] + arguments)
    elif sys.version_info.major == 3:
        result = subprocess.run([exe_path, pathin] + arguments, capture_output=True)

    if sys.version_info.major == 2:
        result = result
    elif sys.version_info.major == 3:
        result = result.returncode
    if result == 0:
        with open(ota_path, 'rb') as f:
            old_ota = f.read()
            file_size = os.path.getsize(ota_path)
            with open(len_test_path, 'wb+') as fw:
                fw.seek(0)
                fw.write(old_ota[0:file_size-4])
                fw.close()

            with open(crc_test_path, 'wb+') as fw:
                fill_data = os.urandom(4)
                fw.seek(0)
                fw.write(old_ota[0:file_size-4])
                fw.write(fill_data)
                fw.close()
            with open(old_chip_path, 'wb+') as fw:
                fill_data = b'\xff' * 4
                fw.seek(0)
                fw.write(old_ota[0:108])
                fw.write(fill_data)
                fw.write(old_ota[112:])
                fw.close()

        with open(pathin, 'rb') as f:
            old_ota = f.read()
            file_size = os.path.getsize(pathin)

            if(partition_size > file_size):
                with open(fill_path, 'wb+') as fw:
                    m = partition_size - file_size
                    fill_data = os.urandom(m)
                    fw.seek(0)
                    fw.write(old_ota)
                    fw.write(fill_data)
                    fw.close()
                    print("fill file complete: %d" % (m))
        if sys.version_info.major == 2:
            result = subprocess.call([exe_path, fill_path] + arguments)
        elif sys.version_info.major == 3:
            result = subprocess.run([exe_path, fill_path] + arguments, capture_output=True)

if __name__ == '__main__':
    if len(sys.argv) == 3:
        fill(sys.argv[1], int(sys.argv[2]))
