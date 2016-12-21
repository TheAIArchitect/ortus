#!/bin/bash

ps o state,command axh | grep "^[SD]" | cut -b 3-
