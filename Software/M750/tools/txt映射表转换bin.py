import struct

# 读取映射表并转换为二进制文件
def txt_to_bin(input_txt, output_bin):
    with open(input_txt, 'r', encoding='utf-8') as txt_file, open(output_bin, 'wb') as bin_file:
        for line in txt_file:
            # 解析每一行，格式为 "GBK_CODE UNICODE_CODE"
            try:
                gbk_code, unicode_code = line.strip().split()
                gbk_code = int(gbk_code, 16)
                unicode_code = int(unicode_code, 16)
                # 使用 struct 打包为 4 字节数据：2 字节 GBK + 2 字节 Unicode
                bin_file.write(struct.pack('<HH', gbk_code, unicode_code))
            except ValueError:
                continue  # 跳过不符合格式的行

# 输入和输出文件路径
input_txt = 'gbk_to_unicode.txt'  # 输入的映射表文本文件
output_bin = 'gbk_to_unicode.bin'  # 输出的二进制文件

# 执行转换
txt_to_bin(input_txt, output_bin)
print(f"转换完成，已生成文件：{output_bin}")
