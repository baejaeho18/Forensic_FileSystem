# Forensic_FileSystem
저수준 파일 시스템 구조 분석 (GMDsoft)
GMDsoft <br>
Cellebrite <br>

Answer the following questions <br>
- exFat의 경우 복사된 파일과 레코딩한 파일의 저장구조가 다르다.
  > 파일 저장 패턴 자체가 Forensic Artifact
- JTAG, Chip-Off시 BitFlip 현상 발생
  > F/S 메타 데이타 손상 가능
- 256G 아이폰의 Full File System을 가져오니 4TB
  > hard link된 동일한 파일 복제

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

### File System Restore

### Mobile File System

### Virtual File System
