if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /home/lucas/GlastExternals/CMT/v1r16p20040701
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=irfs -version=v0r9p2 -path=/home/lucas/Glast/ScienceTools-v7r6p1 $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

