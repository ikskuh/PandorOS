#pragma once

/**
 * Initializes the catalog system.
 */
void catalog_init();

/**
 * Opens the catalog menu.
 */
void catalog_open();

/**
 * Gets the selection of the catalog and resets it.
 * @return Last selection in the catalog menu, returns NULL if none.
 * @remarks On two subsequent calls, the second call will always return NULL.
 */
char const *catalog_get();