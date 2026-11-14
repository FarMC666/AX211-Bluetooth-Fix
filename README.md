# Intel AX211 蓝牙固件修复 · Lenovo Legion Y7000P 2022

适用于本机 Intel AX211（USB `8087:0033`）和 macOS Sequoia 15.8.2（24H232，x86_64）。原固件下蓝牙无法稳定开启，`bluetoothd` 会反复退出。更换 AX211 固件后，已在测试 U 盘冷启动环境完成蓝牙外设发现、首次配对、连接、重连和音频播放验证。

**当前状态为发布候选版。** macOS 和外设验收已通过，另一系统的实际引导待测。内置 EFI 保留原配置。

## 安装包

使用 [候选固件包](dist/AX211-24H232-firmware-candidate.zip)。包内的 `IntelBluetoothFirmware.kext` 包含与本机匹配的 `ibt-0040-0041.sfi` 固件及配套 DDC 配置。[对照包](dist/AX211-24H232-firmware-control.zip) 保留旧固件，供复现原故障使用。

在仓库根目录校验候选包：

```sh
shasum -a 256 dist/AX211-24H232-firmware-candidate.zip
```

预期 SHA-256 为 `1da7f1ec77362282e6a194554a97f4cb55545ebd78313059ed7556fdababd419`。

## 在测试 U 盘上使用

以下步骤适用于已有可启动 OpenCore 测试 U 盘，且已启用 `IntelBluetoothFirmware.kext` 的配置。本机验证时同时加载了 IntelBTPatcher 2.5.0 和 BlueToolFixup 2.7.3。

1. 挂载测试 U 盘的 EFI 分区，核对卷名和设备标识。将原有的完整 `EFI/OC/Kexts/IntelBluetoothFirmware.kext` 复制到 `EFI/OC/Kexts` 以外的备份目录，保留内置 OpenCore 作为恢复入口。
2. 解压候选包，用其中的同名 Kext 替换测试 U 盘上的原件。沿用已有的 `config.plist` 条目、Kext 顺序和其他驱动。
3. 关机后从测试 U 盘冷启动。进入 macOS 后运行下列命令，核对加载的驱动和蓝牙状态：

   ```sh
   kmutil showloaded | grep -F com.zxystd.IntelBluetoothFirmware
   system_profiler SPBluetoothDataType
   ```

   驱动版本仍显示 `2.5.0`。候选包的加载 UUID 应为 `B0661BCB-BBBA-3CCD-A8FE-06C0750785AC`。本机验收时蓝牙为开启状态，控制器地址非零，固件版本显示 `v12568 c12568`。

**回滚：** 从内置 OpenCore 启动 macOS，用第 1 步保存的原始 Kext 恢复测试 U 盘上的同名目录，再关机冷启动。

## 真机验收

| 项目 | 实测结果 |
| --- | --- |
| 蓝牙初始化与稳定性 | 越过原失败点。约 55 分钟后仍是首次启动的 `bluetoothd` 进程，期间未发现新增同签名 IPS。 |
| 索尼 WH-CH520 | Mac 发现耳机并完成**首次配对**，断开后可重连并播放音频。 |
| AirPods Pro 2 | 已通过 iCloud 绑定的耳机可连接。 |
| Wi-Fi 与显示 | itlwm 2.3.0 配合 HeliPort 可正常联网。RTX 3050 Laptop GPU 已识别，两路显示输出在线。 |
| 引导 | 已从测试 U 盘启动 macOS，另一系统引导待测。 |

**已知限制：** iPhone 15 Pro Max 可以发现 Mac；从手机发起配对提示 “MacBook Pro” is not supported，Mac 的蓝牙设置也未列出该手机。蓝牙个人热点尚未验证。AirDrop、Handoff 不在本次适配范围内。

## 修复依据

两份实验包使用相同的驱动代码，85 个嵌入资源中只有本机对应的 SFI 和 DDC 不同。旧固件的命令位图声明仅支持 LE Read Buffer Size V1，macOS 选择 V2 后在本地返回 628 并进入初始化失败路径。候选固件 `BT_Solar_REL82200_23.50.26292.82200` 声明支持 V2，同一系统成功完成初始化。修复通过加载固件生效，无需修改 `bluetoothd`。抓包、固件来源和对照结果见[诊断与验收记录](docs/AX211-24H232-initialization.md)。
