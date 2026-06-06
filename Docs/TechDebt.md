# Tech debt — Rdk-HardwareLib

Legend: `[ ]` open · `[x]` closed · **AU-** = Arduino upload / board profile.

## AU — Arduino upload & board profile

| ID | Description | Opened | Closed (stage) |
|----|-------------|--------|----------------|
| [x] AU-01 | Mega2560 avrdude protocol was stk500v2 instead of wiring | plan | §1 |
| [x] AU-02 | No validate BoardProfile vs bundled HEX before upload | plan | §2 |
| [x] AU-03 | Upload blocks GUI thread (sync RunUpload in MEnv_Calculate) | plan | §3 |
| [x] AU-04 | No live UploadProgress in board panel during flash | plan | §3–§4 |
| [x] AU-05 | No upload target preview / mismatch warning in GUI | plan | §4 |
| [x] AU-06 | No board auto-detect from USB when selecting COM port | plan | §5 |
| [x] AU-07 | SpikeSamples Hardware missing 01–08 / README Uno vs Mega | plan | §6 |
| [x] AU-08 | Transport/GUI docs outdated (protocol, async upload) | plan | §7 |
| [x] AU-09 | Integration test assumes synchronous ProcessBoardEdges upload | plan | §3 |
| [ ] AU-10 | CH340 clones: cannot distinguish Uno vs Mega by USB | plan | — (documented in Firmware/README, GUI) |
| [ ] AU-11 | No cancel-upload / kill avrdude | plan | — (out of v1) |
| [x] AU-12 | Sample configs default BoardProfile=0 while user has Mega on desk | plan | §5–§6 |
