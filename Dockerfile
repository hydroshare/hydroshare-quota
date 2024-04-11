FROM centos:centos7.6.1810 as builder
RUN yum makecache -y && \
    yum clean all&& \
    yum update -y
RUN yum install -y epel-release \
                   yum-utils \
                   vim-enhanced \
                   expect \
                   wget \
                   git
RUN yum install -y pwgen \
                   jq
RUN rpm --import https://packages.irods.org/irods-signing-key.asc && \
    wget -qO - https://packages.irods.org/renci-irods.yum.repo | tee /etc/yum.repos.d/renci-irods.yum.repo
RUN yum makecache -y && \
    yum group install -y "Development Tools" --setopt=group_package_types=mandatory,default,optional
RUN yum install -y --skip-broken irods-externals-*
RUN yum install -y openssl-devel libcurl-devel
RUN yum install -y irods-devel-4.2.11-1
COPY . /hydroshare-quota
# CMakeLists.txt must match iRODS version specified. This file is under version control.
# cmake3.21 for iRODS 4.3.x
# cmake3.11.4-0 for iRODS 4.2.x
WORKDIR /hydroshare-quota/
RUN /opt/irods-externals/cmake3.11.4-0/bin/cmake . 2>&1 | tee cmake.stdout
RUN cat cmake.stdout
RUN make package 2>&1 | tee make.stdout
# Investigate for Rocky/RHEL 9
RUN mkdir /output && mv *.rpm /output/

FROM alpine
COPY --from=builder /output /output