#!/bin/bash

if [[ "X${OPTS}" == "X" ]] ; then
    >&2 echo "ERROR: need to specify one of deps/config.opts/ as OPTS"
    exit 1
fi

export LANG=en_US.UTF-8
echo "127.0.0.1 ${HOSTNAME}" >> /etc/hosts

export VNEV_SITE_PKG_PATH=$(. /venvs/${OPTS}.sh && python -c "import site; print(site.getsitepackages()[0])")
if [[ -e "${VNEV_SITE_PKG_PATH}/dune.xt-nspkg.pth" ]] ; then
    echo '[entrypoint] ensuring lines in dune.*-nspkg.pth are short enough in venv'
    sed "s/;/\n/g" -i "${VNEV_SITE_PKG_PATH}/dune.xt-nspkg.pth"
    sed "s/;/\n/g" -i "${VNEV_SITE_PKG_PATH}/dune.gdt-nspkg.pth"
fi

echo '[entrypoint] use ${DUNECONTROL} to configure and build dune-gdt'
echo

git config --global --add safe.directory /source

if [ "X$@" == "X" ]; then
    exec /bin/bash --rcfile <(cat /root/.bashrc; cat /venvs/${OPTS}.sh)
else
    exec /bin/bash -c ". /venvs/${OPTS}.sh && $@"
fi
