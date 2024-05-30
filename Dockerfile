FROM ubuntu:22.04

RUN apt-get -y update \
	&& apt-get -y upgrade \
	&& apt-get install -y wget

RUN wget https://apt.devkitpro.org/install-devkitpro-pacman
RUN chmod +x ./install-devkitpro-pacman
RUN echo "y" | ./install-devkitpro-pacman
RUN ln -sf /proc/mounts /etc/mtab
RUN printf "\nY\n\n" | dkp-pacman -S gba-dev

RUN apt-get install -y \
	build-essential less git nano ranger bat ripgrep

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV DEVKITPPC=/opt/devkitpro/devkitPPC
ENV PATH=$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH
ENV EDITOR=nano

WORKDIR /srpg-engine

CMD make -kj && cp build/*.h out
