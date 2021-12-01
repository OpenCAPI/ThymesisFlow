/*
 * Copyright 2021 International Business Machines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Christian Pinto - christian.pinto@ibm.com
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef DEBUG
#define DBG_PRINT(fmt,args...) \
	do {printf("\n%s - "fmt, __func__, ##args); fflush(stdout);} while (0)
#else
#define DBG_PRINT(fmt,...) ((void) (0))
#endif

#endif
