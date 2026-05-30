# MultiApp Dockerfile
# A GTKmm 4.0 application with WebKitGTK browser

FROM debian:bookworm-slim

# Set environment variables for non-interactive apt
ENV DEBIAN_FRONTEND=noninteractive

# Install base dependencies, build tools, GTKmm 4.0, WebKitGTK 6.0, and X11 forwarding
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Build essentials
    build-essential \
    pkg-config \
    make \
    g++ \
    libgtkmm-4.0-dev \
    libwebkitgtk-6.0-dev \
    libasio-dev \
    libgtest-dev \
    fontconfig \
    locales \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && localedef -i en_US -c -f UTF-8 en_US.UTF-8

# Set locale
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Set working directory
WORKDIR /app

# Copy source files
COPY . /app/

# Build the application
RUN make clean || true
RUN make release

# Set display environment variable for X11 forwarding
ENV DISPLAY=:0

# Default command - run the application
CMD ["./multiapp.exe"]
