/***
 * Copyright (C) 2024 Mário A. Moiane (connect at imariom dot com)
 * Licensed under the MIT license. See LICENSE.txt in the project root for details.
 *
 */

#define RESTPP_VERSION_MAJOR 1
#define RESTPP_VERSION_MINOR 0
#define RESTPP_VERSION_PATCH 0

// Helper macros for stringification
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define RESTPP_VERSION TOSTRING(RESTPP_VERSION_MAJOR.RESTPP_VERSION_MINOR.RESTPP_VERSION_PATCH)