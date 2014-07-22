file(GLOB _icons *.png)

ecm_install_icons(ICONS ${_icons}
                  DESTINATION ${ICON_INSTALL_DIR})