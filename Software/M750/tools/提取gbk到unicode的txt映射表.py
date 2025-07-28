# 提取 GBK 到 Unicode 的映射表
with open('gbk_to_unicode.txt', 'w', encoding='utf-8') as f:
    for high_byte in range(0x81, 0xFF):  # GBK 高字节范围
        for low_byte in range(0x40, 0xFF):  # GBK 低字节范围
            gbk_code = (high_byte << 8) | low_byte
            try:
                # GBK 编码到 Unicode 解码
                gbk_char = bytes([high_byte, low_byte]).decode('gbk')
                if len(gbk_char) == 1:  # 确保是单个字符
                    unicode_code = ord(gbk_char)
                    f.write(f"{gbk_code:04X} {unicode_code:04X}\n")
            except UnicodeDecodeError:
                continue
