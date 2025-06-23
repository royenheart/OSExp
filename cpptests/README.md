# C++ 特性测试

## move 语义性能测试

个人电脑上的测试结果

| Benchmark              | Time      | CPU       | Iterations |
| ---------------------- | --------- | --------- | ---------- |
| BM_CopyConstruction    | 1293 ns   | 1293 ns   | 552790     |
| BM_MoveConstruction    | 1125 ns   | 1125 ns   | 623670     |
| BM_CopyAssignment      | 1199 ns   | 1199 ns   | 605055     |
| BM_MoveAssignment      | 1168 ns   | 1168 ns   | 600190     |
| BM_ReturnValue_Copy    | 1036 ns   | 1036 ns   | 675736     |
| BM_VectorPushBack_Copy | 307533 ns | 306920 ns | 2266       |
| BM_VectorPushBack_Move | 284125 ns | 283289 ns | 2458       |
| BM_VectorEmplaceBack   | 275589 ns | 274857 ns | 2534       |
| BM_StringCopy          | 1651 ns   | 1650 ns   | 424336     |
| BM_StringMove          | 849 ns    | 849 ns    | 826729     |
| BM_UniquePtrCopy       | 1427 ns   | 1427 ns   | 487815     |
| BM_UniquePtrMove       | 1126 ns   | 1126 ns   | 618211     |