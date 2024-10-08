FROM debian:12

###################################
#
# Usual maintenance
#

# Install debian packages:
# fix location for glib-2.0 (hardcoded in makefile)
RUN set -eux ; \
    apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends vim curl openjdk-17-jdk make xsltproc g++ libgdome2-dev gsoap++ uuid-dev && \
    ln -s /usr/lib/x86_64-linux-gnu/glib-2.0/ /usr/lib/glib-2.0 && \
    apt-get clean && \
    true


# fake source of mcsins/config/mcs.sh so it remains permanent
ENV MCSTOP=/root/MCSTOP/
ENV MCSRELEASE=DEVELOPMENT
ENV MCSENV=default
ENV MCSDATA=${MCSTOP}/data
ENV MCSROOT=${MCSTOP}/${MCSRELEASE}
ENV INTROOT=${MCSROOT}

ENV LD_LIBRARY_PATH=../lib:${MCSROOT}/lib
ENV PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:../bin:${MCSROOT}/bin
ENV MANPATH=/usr/man:/usr/share/man:/usr/local/man:/usr/local/share/man:/usr/X11R6/man:../man:${MCSROOT}/man


# create MCSROOT structure:
# hack to fix and create $MCSDATA/tmp directories required by SearchCal
RUN set -eux ; \
    mkdir -p ${MCSTOP}/etc && \
    mkdir -p ${MCSDATA} && \
    mkdir -p ${MCSROOT}/bin && \
    mkdir -p ${MCSROOT}/lib && \
    mkdir -p ${MCSROOT}/man && \
# specific to SearchCal:
    mkdir -p ${MCSDATA}/tmp/GetCal && \
    mkdir -p ${MCSDATA}/tmp/GetStar && \
    mkdir -p /root/public_html && \
    true


# Set compiler options:
ENV OPTIMIZE=2
ENV DEBUG=
# disable warning Wstringop-overread (mcsBUF...) and use _DEFAULT_SOURCE instead of _GNU_SOURCE
ENV CCS_CFLAGS="-g -Wno-stringop-overread -D_DEFAULT_SOURCE"


# copy MCS git repository
ADD MCS /root/MCS

# C modules:
RUN set -eux ; \
    export MAKE_VERBOSE=1 ; \
    cd /root/MCS && \
    for i in mcsins mkf mcscfg ctoo tat mcs log err misc thrd timlog; do (cd $i/src; make clean all install); done && \
    true

# libxml2 + unicode requires c++11:
ENV CCS_CFLAGS="-g -Wno-stringop-overread -D_DEFAULT_SOURCE -std=c++11"

# C++ modules:
RUN set -eux ; \
    export MAKE_VERBOSE=1 ; \
    cd /root/MCS && \
    for i in misco env cmd msg sdb fnd evh; do (cd $i/src; make clean all install); done && \
    true


# copy SearchCal git repository
ADD SearchCal  /root/SearchCal

ENV CCS_CFLAGS="-g -Wno-stringop-overread -D_DEFAULT_SOURCE"

# C modules:
RUN set -eux ; \
    export MAKE_VERBOSE=1 ; \
    cd /root/SearchCal && \
    for i in alx; do (cd $i/src; make clean all install); done ; \
    true

ENV CCS_CFLAGS="-g -Wno-stringop-overread -D_DEFAULT_SOURCE -std=c++11"

# C++ modules:
RUN set -eux ; \
    export MAKE_VERBOSE=1 ; \
    cd /root/SearchCal && \
# Finish to hack namespaces field to compile sclws module:
    sed -i~ "s/Namespace\* namespaces;/Namespace namespaces \[0\];/g" sclws/src/sclwsServer.cpp && \
    for i in vobs simcli sclsvr sclws; do (cd $i/src; make clean all install); done ; \
    true


# expose the default port of the server
EXPOSE 8079

# define sclwsServer as the default command
# use exec syntax [] to let CMD handle signals:
CMD ["sclwsServer", "-l", "0", "-v", "3"]

WORKDIR /root/

ENV VOBS_DEV_FLAG=0
ENV VOBS_VIZIER_URI=https://vizier.cds.unistra.fr/
ENV VOBS_LOW_MEM_FLAG=1

# test can be perfomed directly from SearchCal GUI :
# - set http://localhost:8079 in the server.url.address preference
# - run $ docker run -p 8079:8079 ...
# - run SearchCal GUI


