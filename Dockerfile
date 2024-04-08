FROM centos:centos7.6.1810
RUN yum -y install epel-release &&\
    yum makecache -y &&\
    yum clean all &&\
    yum update -y
RUN yum install -y epel-release \
    yum-utils \
    emacs \
    vim-enhanced \
    expect \
    wget \
    git

RUN yum install -y epel-release \
    pwgen \
    jq

RUN rpm --import https://packages.irods.org/irods-signing-key.asc
RUN wget -qO - https://packages.irods.org/renci-irods.yum.repo | \
    tee /etc/yum.repos.d/renci-irods.yum.repo
RUN yum makecache -y fast &&\
    yum group install -y "Development Tools" --setopt=group_package_types=mandatory,default,optional

RUN yum install -y sudo python3 centos-release-scl
RUN yum install -y python36-distro devtoolset-10-gcc devtoolset-10-gcc-c++
RUN yum install -y irods-externals-*
# https://github.com/irods/externals?tab=readme-ov-file#rhel--centos-7
RUN git clone https://github.com/irods/externals.git &&\
    cd externals &&\
    ./install_prerequisites.py &&\
    scl enable devtoolset-10 bash
RUN make
RUN yum install -y openssl-devel libcurl-devel
RUN yum install -y irods-devel-4.2.6-1

COPY . /hydroshare-quota

WORKDIR /hydroshare-quota
RUN /opt/irods-externals/cmake3.11.4-0/bin/cmake .
RUN make package

# save hydroshare-quota-microservices-centos7-x86_64.rpm