# Install

requires brpc

```bash
sudo apt install -y git g++ make cmake libssl-dev libgflags-dev libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev
```

## 测试报告

### 报告1

- 机器环境：

| 部件            | 配置                                              |
| --------------- | ------------------------------------------------- |
| CPU             | AMD Ryzen 7 8845H                                 |
| Hyper Threading | ON                                                |
| CPU Cores       | 1 sockets * 8 cores * 2 threads = 16 cores        |
| Cache           | L1d: 256 KiB / L1i: 256KiB / L2: 8MiB / L3: 16MiB |

- 测试命令：

```bash
./dbd_test -duration 20 -num_readers 1000 -num_writers 10
```

- 测试结果

| 方法                   | Write_throughput(w/s) | Read_throughput(r/s) | Avg_latency(ns) |
| ---------------------- | --------------------- | -------------------- | --------------- |
| DoublyBufferedData     | 83.1638               | 2816290              | 2816290         |
| Shared Mutex (RW Lock) | 559.916               | 58204.3              | 396.848         |
| Simple Mutex           | 587.275               | 139432               | 366.736         |