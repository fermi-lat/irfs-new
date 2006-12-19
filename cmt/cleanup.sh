if test "${CMTROOT}" = ""; then
  CMTROOT=/home/lucas/GlastExternals/CMT/v1r16p20040701; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet cleanup -sh -pack=irfs -version=v0r9p2 -path=/home/lucas/Glast/ScienceTools-v7r6p1 $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

