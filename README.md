
# Fission2019-Unified-Analysis-Framework
A grobal unified analysis framework to analyze experiment Fission2019 in RIBLL1 in LANZHOU,

This framework is desiged for Fission2019 data Analysis

Contributor: Fenhai Guan(1)

(1) gfh16@mails.tsinghua.edu.cn

Personal information could be found at the link:
http://inspirehep.net/author/profile/Fen.Hai.Guan.1

目录
=====
<!--ts-->
* [数据转换](#数据转换)
  * [将原始数据(二进制文件)转换成.root文件](#将原始数据二进制文件转换成.root文件)
  * [将RawRoot数据转换成MapRoot数据](#将RawRoot数据转换成 MapRoot数据)
* [数据质检](#数据质检)
  * [SetBranchAddress方法](#SetBranchAddress方法)
  * [TTreeReader方法](#TTreeReader方法)
* [PPAC数据处理](#PPAC数据处理)
* [SSD数据处理](#SSD数据处理)
  * [数据刻度](#数据刻度)
    * [Energy Calibration](#Energy-Calibration)
    * [Hit Multiplicity](#Hit-Multiplicity)
    * [CsI Energy Calibration](#CsI-Energy-Calibration)
    * [Hit Pixellation](#Hit-Pixellatio)
    * [Particle Identification](#Particle-Identification)
  * [物理分析](#物理分析)
<!--te-->






## 数据转换

###    1. 将原始数据(二进制文件)转换成.root文件 (需要利用DAQ配置)
####       (1)修改文件：/RIBLLVMEDAQ/Raw2ROOT.cpp
####       (2)目的：将原始数据(二进制文件)转换成.root文件
####       (3)操作：批量转换
#####          <1> 输入文件(原始文件)都在 /vmedata/文件夹下
#####          <2> 在 /vmedata/中 添加 listfilename 文件，将需要格式转换的原始文件名一一写出，每个文件名占一行
#####          <3> 修改 /RIBLLVMEDAQ/下的 Raw2ROOT.cpp文件：目的是修改输出文件的位置输出文件(.root文件)都在 /rootdata/文件加下 (原因：个人更倾向于将输出文件放在/rootdata/文件夹下)
#####          <4> 执行：编译成功后, ./raw2roo.sh listfilename


   2. 将RawRoot数据转换成 MapRoot数据 (需要利用探测器Map)
      (1)修改文件：/RIBLLVMEDAQ/ReadRootFile2D.cpp
      (2)目的：第一步得到的.root文件数据以 T103000 等命名，第二步需要利用探测器Map将每个插件对应的探测器还原出来, 以PPAC1_T 等命名
      (3)操作
         <1>修改/RIBLLVMEDAQ/ReadRootFile2D.cpp 文件
         <2>假定输入文件都在 /rootdata/中，在/rootdata/下添加listfilename 文件,将需要转换的文件名称一一列出，每个文件名占一行
         <3>为避免与原始的.root文件混淆，转换后的.root文件需要另起名称，且最好输出到不同的文件夹下
      (4)执行: 编译成功后, ./ReadRootFile2D listfilename


***************************************************************************
二. 数据质检：画出每个探测器ch的直方图，将所有的直方图存成.root文件.步骤如下：
***************************************************************************
    1. 读取.root文件的tree数据.有两种方法：
       <1>方法1: 传统的setBranchAddress()的方法读取tree结构. 【QC_BranchAdress.cpp】
       <2>方法2：TTreeReader方法更方便读取tree结构           【QC_ReadTree.cpp】

    2. 定义、填充直方图
    3. 存储所有的Hist，输出pdf到文件, 方便肉眼进行质检
    4. 存储所有的Hist,写入.root文件. 这一步是为了:一旦pdf文件中的谱有问题，马上可以查看.root文件中对应的直方图
    5. 编译 QC_ReadTree.cpp 后执行, ./QC_ReadTree.cpp listfilename


************************
三. PPAC数据处理
************************


***********************
四. SSD数据处理
***********************

(一)数据刻度
    1. Energy Calibration
       (1)FindPedestal
       (2)PulserCali_AutoFindPeak
       (3)PulserCali_LinearFit
       (4)AlphaCali_FindPeak
       (5)Energy_Calibration




    2. Hit Multiplicity
    3. CsI Energy Calibration
    4. Hit Pixellation
    5. Particle Identification
