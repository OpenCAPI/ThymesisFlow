# Welcome to the Home of ThymesisFlow
Thymesisflow is a HW-SW co-designed prototype enabling
hardware disaggregation of compute resources on POWER9 processor systems.
The current design supports disaggregation of memory by "stealing" it from a
neighbour node, and is based on OpenCAPI.

![ThymesisFlow Architecture](./reference_design_doc/thymesis_arch.jpg)

The ThymesisFlow approach consists of a compute node (left side of the figure)
that is stealing the memory, and a memory node (righ side of the figure) from
which the memory is stolen. The compute endpoint of the ThymesisFlow design is
based on the OpenCAPI M1 mode (or LPC), while the memory endpointi uses the
OpenCAPI C1 mode.  On the compute endpoint the disaggregated memory is mapped at
a specific range of addresses in the physical address space and can be
dynamically hotplugged to a running Linux system. No software modification is
needed to access disaggregated memory, neither in the Linux kernel nor from the
user applications.

This repository contains the complete design (Verilog/HLS) of memory and compute
side, including the OpenCAPI 3.0 reference design.

Please refer to the [ThymesisFlow documentation](./reference_design_doc/ThymesisFlow%20Design%20and%20Deployment%20v1.1.pdf) for a complete description of the
design and its bringup.

## Supported Cards

The ThymesisFlow design currently supports the following cards:

- [Alphadata 9V3](https://www.alpha-data.com/dcp/products.php?product=adm-pcie-9v3)

## How-to bring-up the design
The ThymesisFlow build and bring-up procedure is described in detail in the [design reference manual](./reference_design_doc/ThymesisFlow%20Design%20and%20Deployment%20v1.1.pdf).
Please, refer also to the [OpenCAPI3.0
Wiki](https://github.com/OpenCAPI/OpenCAiPI3.0_Client_RefDesign/wiki) for further details on the Vivado projects creation.

### Create Compute side Vivado project
```console
vivado -source create_project.tcl -tclargs --speed 20.0 --afu thymesisflow --buffer bypass --tftype compute
```

### Create Memory side Vivado project
```console
vivado -source create_project.tcl -tclargs --speed 20.0 --afu thymesisflow --buffer bypass --tftype memory
```

## Resources:

- [ThymesisFlow at the OpenPOWER Summit North America 2019](https://www.youtube.com/watch?v=XcjRL3Lh8Ig)
- [ThymesisFlow at the OpenPOWER Summit Europe 2018](https://www.youtube.com/watch?v=vSKUeGeEkoA)

## What's Next

Our immediate next steps are related to the release of the software support for
ThymesisFlow:

- Memory stealing process: this is the user application that allocates a buffer in
  the memory node for access from a remote node.
- ThymesisStats: Software Library for accessing performance countersi embedded in the design.
- ThymesisFlow orchestration software: we gave a stub at our vision of a
  disaggregated system, and came up with a proposal control plane for
  dynamic orchestration of disaggregated memory segments.
- Stay tuned!
