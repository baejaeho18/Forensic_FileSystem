# Forensic_FileSystem
저수준 파일 시스템 구조 분석 (GMDsoft) <br>
GMDsoft <br>
Cellebrite <br>

Answer the following questions <br>
- exFat의 경우 복사된 파일과 레코딩한 파일의 저장구조가 다르다.
  > 파일 저장 패턴 자체가 Forensic Artifact
- JTAG, Chip-Off시 BitFlip 현상 발생
  > F/S 메타 데이타 손상 가능
- 256G 아이폰의 Full File System을 가져오니 4TB
  > hard link된 파일 복제

## File System
The **structure and logic rule** used to manage the groups of information and their names
- ext3/4, hfs+, ntfs
- fat 12/16/32, exFAT
- efs2, tfs4, yaffs, jffs2
- f2fs, apfs, vdfs

* Partition : 물리적으로 연속된 섹터의 모임
  - 백업 용이, 다종 운영체제 설치 가능, 
* Volumn : 논리적인 섹터의 모임
  - 동적볼륨 : 동적으로 사용량 증가 및 추가/저거 -> 오라클
* Slack : 물리적구조와 논리적구조의 차이로 낭비되는 공간
  - 삭제 데이터 복구 : slack에 이전 데이터가 남아 있을 수 있음
  - 안티 포렌식 : slack에 숨기기
  - 안티 안티 포렌식

* Sector : 하드디스크에서 원자성을 지원하는 I/O 단위 (0x200 : 512B)
* Cluster : MS에서 만든 파일시스템의 I/O단위
* Block/Page
  - Super Block : block size, 각 영역 위치, root iNode number, # of iNodes, F/S size
  - Bitmap : data block 상의 비할당 영역(0 bit) 식별
  - iNode table : 데이터 정보(파일/디렉토리, MAC시간, 크기, 이름, 위치)
  - Data Block

* 저널 : 파일시스템의 변경을 저장하고 있는 파일시스템의 영역 [[wiki]](https://en.wikipedia.org/wiki/Journaling_file_system)
  - 메타 데이터 or 파일 데이터
  - circular queue
  - 파일시스템 장애복원 및 성능향상
* soft(symbolic) link : 원본에 대한 경로(독립된 iNode)
* hard link : 원본(iNode 공유, reference count++)

### File System Restore
Block의 메타데이터를 읽고 iNode table 상의 모든 파일 구조를 추적함. <br>
활성화된 파일과 삭제된 파일을 확인함. <br>
BitMap과 각 할당 파일을 교차검증함. <br>

### Mobile File System
FAT 12/16/32 파일시스템 복원 : SD 카드에서 사용, cluster chain <br> 
Ext 3 : Linux kernel, FFS(Fast File System - 하드디스크 물리적주소와 최적화), block indirect pointer로 데이터 표현 <br>
Ext 4 : extent(start,size)롷 데이터 표현 <br> 
HFS+ <br>
f2fs : NAND Flash에 최적화, indirect block pointer로 회귀 <br>
exFAT : SD카드, no TRIM, fat chain(recording) + extent(image) <br>

### Virtual File System
서로 다른 파일 시스템에 대해 사용자에게 동일한 인터페이스를 제공하는 모듈

## FAT
File Allocation Table <br>
구조가 간단하여 일반 시스템 외에도 이동식 저장장치에서 사용 <br>
파일시스템 복원 : dump 이미지로부터 파일 구조를 파악 <br>

