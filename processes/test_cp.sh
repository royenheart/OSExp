#!/bin/bash

# 设置测试目录和结果目录
src_dir="cptests/src"
dest_dir="cptests/dest"

compare_directories() {
    local src=$1
    local dest=$2

    # 遍历源目录中的所有文件和子目录
    while IFS= read -r -d '' src_file; do
        local rel_path=${src_file#$src}
        local dest_file=$dest$rel_path

        # 获取源文件的属主、权限等信息
        src_owner=$(stat -c "%U" "$src_file")
        src_permissions=$(stat -c "%a" "$src_file")

        # 获取目标文件的属主、权限等信息
        dest_owner=$(stat -c "%U" "$dest_file")
        dest_permissions=$(stat -c "%a" "$dest_file")

        # 比较属主和权限
        if [[ "$src_owner" == "$dest_owner" ]]; then
            echo "属主 $rel_path 相同"
        else
            echo "属主 $rel_path 不同，测试未通过"
            exit -1
        fi

        if [[ "$src_permissions" == "$dest_permissions" ]]; then
            echo "权限 $rel_path 相同"
        else
            echo "权限 $rel_path 不同，测试未通过"
            exit -1
        fi

    done < <(find "$src" -type f -print0)

    # 递归比较子目录
    if [[ $src_subdir != "" ]]; then
        while IFS= read -r -d '' src_d; do
            local rel_path=${src_d#$src}
            local dest_subdir=$dest$rel_path
            local src_subdir=$src$rel_path

            # 检查目标子目录是否存在
            if [[ ! -d "$dest_subdir" ]]; then
                echo "目标子目录 $rel_path 不存在"
                continue
            fi

            compare_directories "$src_subdir" "$dest_subdir"

        done < <(find "$src_subdir" -type d -print0)
    fi
}

# 编译

mkdir -p build
cd build
cmake .. && make -j

root=`pwd`

# 创建测试目录
mkdir -p $src_dir/
rm -rf $src_dir/*
mkdir -p $src_dir/src
touch $src_dir/src/aa.txt
echo "Hello World" > $src_dir/src/aa.txt
touch $src_dir/bb.txt
echo "World" > $src_dir/bb.txt
cp ./cp $src_dir/
cd $src_dir/
ln -s src/aa.txt aasoftln 
ln bb.txt bbhardln 

cd $root

# 清空结果目录
mkdir -p $dest_dir
rm -rf $dest_dir/*

# 运行程序
echo "运行 cp 测试"
./cp $src_dir $dest_dir

# 检查结果目录中的文件和符号链接
echo "检查文件和符号链接..."

# 比较源目录和结果目录中的结构、各个文件的内容是否一致
dif_r=`diff -rq $src_dir $dest_dir`
if [[ $dif_r == "只在 cptests/src 存在：aasoftln" ]]; then
    echo "目录结构相同"
    echo $dif_r
else 
    echo "目录结构不同，测试未通过"
    echo $dif_r
    exit -1
fi

# 检查符号链接
if [[ -e "$dest_dir/aasoftln" ]]; then
    echo "符号链接未被跳过，测试未通过"
    exit -1
else
    echo "符号链接已被跳过"
fi

# 检查属主和权限

compare_directories $src_dir $dest_dir

echo "测试通过"