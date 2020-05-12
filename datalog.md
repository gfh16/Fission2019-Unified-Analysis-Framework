Fission2019 实验数据说明文档
==========================

>+  [硅条探测器数据处理流程教程-word文档](https://cloud.tsinghua.edu.cn/f/ccdef129f9434a26b0a2/)
>+ [硅条探测器数据处理问题总结-PPT文档](https://cloud.tsinghua.edu.cn/f/da72d122784c41c394f7/)

目录
-----
[toc]


-----------------------------------------------
## <font color=#DC143C> 一. SSD能量刻度总结 </font>
>+ [gfh-硅条能量刻度问题总结-PPT-文档](https://cloud.tsinghua.edu.cn/f/00535cc5dcaa4663b8c4/)










-------------------------------------------------------------------
## <font color=#DC143C> 二. 实验数据文件说明 </font>
>+ [gfh-Fission2019实验记录本](https://cloud.tsinghua.edu.cn/f/b79406dc24744e58bae7/)
>+ [gfh-Fission2019实验值班记录表格](https://cloud.tsinghua.edu.cn/f/077701365a754ca68e11/)
>+ [gfh-Fission2019实验文件说明-excel统计](https://cloud.tsinghua.edu.cn/f/55754e979f7845df9678/)

>+ 探测器刻度分：SSD刻度 与 PPAC刻度
>+ SSD 刻度包括: Pulser 刻度 与 $\alpha$ 源刻度
>+ PPAC 刻度包括: 束流刻度 与 $^{252}\rm{Cf}$ 源刻度

### <font color=#00BBDD> 2.1 硅条 Pulser 刻度文件 </font>
| Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
|:------------|:--------------|:-----------:|:---------|
|SSD1_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50| 8 |衰减100倍CO4020闪亮|
|SSD2_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20      | 6 |
|SSD3_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10         | 5 |
|SSD4_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10         | 5 |
|SSD1_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50,100  | 9 |
|SSD2_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50      | 8 |
|SSD3_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20            | 6 |
|SSD4_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50      | 8 |
|SSD1_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5 | 11 | 
|SSD2_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5 | 11 |
|SSD3_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1     | 10 |
|SSD4_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1     | 10 | 0.5的统计～20！SSD4_L1刻度时有噪声 |
|SSD1_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 | 
|SSD2_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |T115000,T115001信号是其他的一半|
|SSD3_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |
|SSD4_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |

### <font color=#00BBDD> 2.2 硅条 $\alpha$ 刻度文件 </font>
>+ **SSD的Layer1刻度时,作了以下改变:**
   SSD1_L1主放gain修改: 由 2 × 4   变成 <font color=#DC143C> 2 × 20 </font>
   SSD2_L1主放gain修改: 由 2 × 7.4 变成 <font color=#DC143C> 2 × 20 </font>


>+ <font color=red> 第一次开靶室: SSD_L1 $\alpha$ 刻度 </font>
   备注: SSD3、SSD4部分 strips 计数率过低！

| Alpha刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|SSD_L1_AlphaCali.0000| 同时刻度4块SSD的Layer1 | SSD1_L1计数正常 |
|SSD_L1_AlphaCali.0001| 同时刻度4块SSD的Layer1 | SSD2_L1计数正常 |
|SSD_L1_AlphaCali.0003| 同时刻度4块SSD的Layer1 | SSD4_L1 CH0无计数(实验中也无计数) |
|SSD_L1_AlphaCali.0004| 同时刻度4块SSD的Layer1 |


>+ <font color=red> 第二次开靶室: SSD_L1 $\alpha$ 刻度 </font>
   备注: 改变 $\alpha$ 源的位置, 主要对 SSD3, SSD4 进行刻度!

| Alpha刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|SSD_L1_AlphaCali.0005| 主要刻度SSD3_L1,SSD4_L1| 第一次Alpha刻度部分无计数,开靶盖第二次刻度|
|SSD_L1_AlphaCali.0006| 主要刻度SSD3_L1,SSD4_L1| 第一次Alpha刻度部分无计数,开靶盖第二次刻度|
|SSD_L1_AlphaCali.0007| 主要刻度SSD3_L1,SSD4_L1| 第一次Alpha刻度部分无计数,开靶盖第二次刻度|
|SSD_L1_AlphaCali.0008| 主要刻度SSD3_L1,SSD4_L1| 第一次Alpha刻度部分无计数,开靶盖第二次刻度|


>+ <font color=red> 第三次开靶室: SSD_L2 $\alpha$ 刻度 </font>
   备注: T112014统计很低！ T121013 & T121014信号异常！

| Alpha刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|SSD_L2_AlphaCali.0000| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0001| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0002| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0003| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0004| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0005| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0006| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0007| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0008| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0009| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0010| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0011| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0012| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0013| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0014| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0015| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0016| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0017| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0018| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0019| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0020| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0021| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0022| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0023| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0024| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0025| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0026| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0027|SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0028| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0029| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0030| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0031| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0032| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|


>+ <font color=red> 第四次开靶室: SSD_L2 $\alpha$ 刻度 </font>
   备注: 为了刻度 T112014. 不过此时 T112028 统计很低！

| Alpha刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|SSD_L2_AlphaCali.0033| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0034| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0035| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0036| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0037| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0038| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0039| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0040| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0041| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0042| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0043| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0044| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0045| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0046| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0047| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|
|SSD_L2_AlphaCali.0048| SSD1的Trig: SSD1_L2B | T112028统计很低！ T121013 & T121014信号异常！|



### <font color=#00BBDD>  2.3 PPAC 束流刻度 </font>
### <font color=#00BBDD>  2.4 PPAC $^{252}\rm{Cf}$ 刻度 </font>
### <font color=#00BBDD>  2.5 实验数据文件 </font>