Fission2019 实验数据说明文档
==========================

>+  [硅条探测器数据处理流程教程-word文档](https://cloud.tsinghua.edu.cn/f/ccdef129f9434a26b0a2/)


目录
-----
[toc]



## <font color=#DC143C> 一. SSD能量刻度总结 </font>
>+ [gfh-硅条能量刻度问题总结-PPT-文档](https://cloud.tsinghua.edu.cn/f/00535cc5dcaa4663b8c4/)









## <font color=#DC143C> 二. 实验数据文件说明 </font>
>+ [gfh-Fission2019实验记录本](https://cloud.tsinghua.edu.cn/f/b79406dc24744e58bae7/)
>+ [gfh-Fission2019实验值班记录表格](https://cloud.tsinghua.edu.cn/f/077701365a754ca68e11/)
>+ [gfh-Fission2019实验文件说明-excel统计](https://cloud.tsinghua.edu.cn/f/55754e979f7845df9678/)

>+ 探测器刻度分：SSD刻度 与 PPAC刻度
>+ SSD 刻度包括: Pulser 刻度 与 $\alpha$ 源刻度
>+ PPAC 刻度包括: 束流刻度 与 $^{252}\rm{Cf}$ 源刻度


### <font color=#00BBDD> 2.1 硅条刻度文件 </font>
#### <font color=#FF00FF> 2.1.1 硅条 Pulser 刻度 </font>

+ **SSD_PulserCali_Pedestal.0000**
  >+ 刻度 SSD_L1 与 SSD_L2的零点道

  | Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
  |:------------|:--------------|:-----------:|:---------|
  |SSD_PulserCali_Pedestal.0000| 同时刻度L1, L2的零点道 | 1 |
&emsp;
+ **SSD_L1 拨档法(Switch)**
  >+ 拨档法, 即使用精密脉冲发生器 Ortec-419 前面板的拨档进行信号衰减！
  >+ $\alpha$ 源对 SSD_L1 进行刻度时, 发现 SSD1,SSD2 信号很小甚至看不到, 因此需要改变主放的 gain, 同时 Pulser 刻度也需要重新刻度.
  >+  SSD1_L1主放gain修改: 由 2 × 4  $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>
   SSD2_L1主放gain修改: 由 2 × 7.4 $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>

  | Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
  |:------------|:--------------|:-----------:|:---------|
  |SSD1_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50| 8 |衰减100倍CO4020闪亮|
  |SSD2_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20      | 6 |
  |SSD3_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10         | 5 |
  |SSD4_L1_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10         | 5 |
  |SSD1_L1_PulserReCali_Gain4_Switch.0000| 衰减因子: 1,2,4,5,10,20,40,50 | 8 ||
  |SSD1_L1_PulserReCali_Gain20_Switch.0000| 衰减因子: 4,5,10,20,40,50 | 6 ||
  |SSD2_L1_PulserReCali_Gain7.4_Switch.0000| 衰减因子: 1,2,4,5,10,20| 6 ||
  |SSD2_L1_PulserReCali_Gain20_Switch.0000| 衰减因子: 4,5,10,20,40| 5 ||


&emsp;
+ **SSD_L1 等间距法(Height)**
  >+ 等间距法, 是指用 Ortec-419 前面板的旋钮进行等间距信号衰减！
  >+ $\alpha$ 源对 SSD_L1 进行刻度时, 发现 SSD1,SSD2 信号很小甚至看不到, 因此需要改变主放的 gain, 同时 Pulser 刻度也需要重新刻度.
  >+  SSD1_L1主放gain修改: 由 2 × 4  $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>
   SSD2_L1主放gain修改: 由 2 × 7.4 $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>

  | Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
  |:------------|:--------------|:-----------:|:---------|
  |SSD1_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5 | 11 | 刻度文件正常 |
  |SSD2_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5 | 11 | 刻度文件正常 |
  |SSD3_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1     | 10 | 刻度文件正常 |
  |SSD4_L1_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1     | 10 | 0.5的统计～20！SSD4_L1刻度时有噪声 |
  |SSD1_L1_PulserReCali_Gain4_Height.0000| 相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |  刻度文件正常 |
  |SSD1_L1_PulserReCali_Gain20_Height.0000| 相对幅度: 10,9,8,7,6,5,4,3,2,1 | 10 |  刻度文件正常 |
  |SSD2_L1_PulserReCali_Gain7.4_Height.0000| 相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |  刻度文件正常 |
  |SSD2_L1_PulserReCali_Gain20_Height.0000| 相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |  刻度文件正常 |


&emsp;
+ **SSD_L2 拨档法(Switch)**
  >+ 拨档法, 即使用精密脉冲发生器 Ortec-419 前面板的拨档进行衰减！

  | Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
  |:------------|:--------------|:-----------:|:---------|
  |SSD1_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50,100  | 9 | 刻度文件正常 |
  |SSD2_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50      | 8 | 刻度文件正常 | 
  |SSD3_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20       | 6 | 刻度文件正常 |
  |SSD4_L2_PulserCali_Switch.0000|衰减因子: 1,2,4,5,10,20,40,50 | 8 | 刻度文件正常 |

&emsp;
+ **SSD_L2 等间距法(Height)**
  >+ 等间距法, 是指用 Ortec-419 前面板的旋钮进行等间距信号衰减！

  | Pulser刻度  |   刻度条件说明  |   刻度点数目    | 备注|
  |:------------|:--------------|:-----------:|:---------|
  |SSD1_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 | 刻度文件正常 |
  |SSD2_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 |T115000,T115001信号是其他的一半|
  |SSD3_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 | 刻度文件正常 | 
  |SSD4_L2_PulserCali_Height.0000|相对幅度: 10,9,8,7,6,5,4,3,2,1,0.5| 11 | 刻度文件正常 | 


&emsp;
#### <font color=#FF00FF> 2.1.2 硅条 $\alpha$ 刻度文件 </font>

+ **硅条 L1 刻度**
  >+ **SSD的Layer1刻度时,作了以下改变:**
   SSD1_L1主放gain修改: 由 2 × 4  $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>
   SSD2_L1主放gain修改: 由 2 × 7.4 $\Rightarrow$ <font color=#DC143C> 2 × 20 </font>

  >+ <font color=red> 第一次开靶室: SSD_L1 $\alpha$ 刻度 </font>
   备注1: SSD3、SSD4部分 strips 计数率过低！
   备注2：T120000 空谱, 即 SSD4_L1_CH0 为空, 实验中也是空谱！

  | Alpha刻度  |   刻度条件说明    | 备注 |
  |:------------|:--------------|:---------|
  |SSD_L1_AlphaCali.0000| Trigger: SSD 单举 | SSD3_L1, SSD4_L1 部分统计低, T120000空谱 |
  |SSD_L1_AlphaCali.0001| Trigger: SSD 单举 | SSD3_L1, SSD4_L1 部分统计低, T120000空谱 |
  |SSD_L1_AlphaCali.0003| Trigger: SSD 单举 | SSD3_L1, SSD4_L1 部分统计低, T120000空谱 |
  |SSD_L1_AlphaCali.0004| Trigger: SSD 单举 | SSD3_L1, SSD4_L1 部分统计低, T120000空谱 |


  >+ <font color=red> 第二次开靶室: SSD_L1 $\alpha$ 刻度 </font>
     备注1: 改变 $\alpha$ 源的位置, 主要对 SSD3, SSD4 进行刻度!
     备注2: T120000 空谱, 即 SSD4_L1_CH0 为空, 实验中也是空谱！

  | Alpha刻度  |   刻度条件说明    | 备注 |
  |:------------|:--------------|:---------|
  |SSD_L1_AlphaCali.0005| Trigger: SSD 单举 | 主要对SSD3与SSD4 重新刻度 |
  |SSD_L1_AlphaCali.0006| Trigger: SSD 单举 | 主要对SSD3与SSD4 重新刻度 |
  |SSD_L1_AlphaCali.0007| Trigger: SSD 单举 | 主要对SSD3与SSD4 重新刻度 |
  |SSD_L1_AlphaCali.0008| Trigger: SSD 单举 | 主要对SSD3与SSD4 重新刻度 |


&emsp;
+ **硅条 L2 刻度**
  >+ <font color=red> 第三次开靶室: SSD_L2 $\alpha$ 刻度 </font>
     备注1: T112014统计很低！
     备注2: T121013 与 T121014 出现两组“三峰”, 分别在 [150,180] 与 [330,380]

  | Alpha刻度  |   刻度条件说明    | 备注 |
  |:------------|:--------------|:---------|
  |SSD_L2_AlphaCali.0000| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0001| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0002| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0003| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0004| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0005| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0006| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0007| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0008| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0009| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0010| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0011| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0012| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0013| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0014| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0015| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0016| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0017| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0018| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0019| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0020| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0021| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0022| SSD1的Trig: SSD1_L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0023| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0024| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0025| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0026| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0027| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0028| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0029| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0030| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0031| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0032| Trigger: SSD 单举-L2F | T112014统计很低！ T121013 & T121014信号异常！|


  &emsp;
  >+ <font color=red> 第四次开靶室: SSD_L2 $\alpha$ 刻度 </font>
     备注1: 为了刻度 T112014. 不过此时 T112028 统计很低！
     备注2: T121013 与 T121014 出现两组“三峰”, 分别在 [150,180] 与 [330,380]

  | Alpha刻度  |   刻度条件说明    | 备注 |
  |:------------|:--------------|:---------|
  |SSD_L2_AlphaCali.0033| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0034| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0035| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0036| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0037| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0038| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0039| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0040| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0041| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0042| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0043| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0044| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0045| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0046| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0047| STrigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|
  |SSD_L2_AlphaCali.0048| Trigger: SSD 单举-L2B | T112028统计很低！ T121013 & T121014信号异常！|



### <font color=#00BBDD> 2.2 PPAC 刻度文件 </font>
>+ PPAC1 只有束流刻度
>+ PPAC2 束流刻度与 Cf252 放射源刻度
>+ PPAC3 只有 Cf252 放射源刻度 

#### <font color=#FF00FF>  2.2.1 PPAC 束流刻度 </font>

| PPAC 束流刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|PPAC1_BeamCalibration.0000| Trigger: PPAC 单举 | 数据无效： 开DAQ时, PPAC1 噪声起来了|
|PPAC1_BeamCalibration.0001| Trigger: PPAC 单举 | PPAC1 -465V, PPAC2,3电压0 |
|PPAC1_BeamCalibration.0002| Trigger: PPAC 单举 | PPAC1 -465V, PPAC2,3电压0 |
|PPAC2_BeamCalibration.0000| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC2_BeamCalibration.0001| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_BeamCalibration.0000| Trigger: PPAC 单举 | 统计过低, 放弃束流刻度! |


#### <font color=#FF00FF>  2.2.2 PPAC $^{252}\rm{Cf}$ 刻度 </font>
| PPAC $^{252}\rm{Cf}$ 刻度  |   刻度条件说明    | 备注 |
|:------------|:--------------|:---------|
|PPAC2_Cf252Calibration.0000| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0000| Trigger: PPAC 单举 | PPAC3 -460V, PPAC1,2电压0 |
|PPAC3_Cf252Calibration.0001| Trigger: PPAC 单举 | PPAC3 -460V, PPAC1,2电压0 |
|PPAC3_Cf252Calibration.0002| Trigger: PPAC 单举 | PPAC3 -460V, PPAC1,2电压0 |
|PPAC3_Cf252Calibration.0003| Trigger: PPAC 单举 | PPAC3 -460V, PPAC1,2电压0 |
|PPAC3_Cf252Calibration.0004| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0005| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0006| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0007| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0008| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0009| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0010| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0011| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |
|PPAC3_Cf252Calibration.0012| Trigger: PPAC 单举 | PPAC2 -460V, PPAC1,3电压0 |


### <font color=#00BBDD>  2.3 实验数据文件 </font>