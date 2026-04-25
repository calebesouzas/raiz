#pragma once

/**
 * @doc!
 * @name(header) voosh.h
 * @desc all declarations for Voosh's API
 */

/**
 * @doc!
 * @name(type) CmdKind
 * @desc commands enumerator
 */
typedef enum {
  VOOSH_CMD_INVALID = 0,
  VOOSH_CMD_BUILD,
  VOOSH_CMD_RUN,
  __voosh_cmd_count,
} Voosh_CmdKind;

/**
 * @doc!
 * @name(type) Flag
 * @desc enumerator where variants are bit masks.
 * This is to store multiple booleans in one single `int` variable.
 */
typedef enum {
  VOOSH_FLAG_INVALID  = 0,
  VOOSH_FLAG_STRICT   = (1 << 0),
  VOOSH_FLAG_NO_BUILD = (1 << 1),
  // @note can't use flag count for static asserts D:
} Voosh_Flag;

/**
 * @doc!
 * @name(type) Cmd
 * @desc unify [@CmdKind] and `int flags` field using [@Flag]
 */
typedef struct {
  Voosh_CmdKind kind;
  int flags;
} Voosh_Cmd;

/**
 * @doc!
 * @name(function) parse_args
 * @desc turns command line arguments into a [@Cmd] with [@Flag]s
 */
Voosh_Cmd voosh_parse_args(int argc, char **argv, char ***raiz_args);

/**
 * @doc!
 * @name(function) rebuild_urself
 * @params `int argc, char **argv, char **envp` common entry point arguments
 * @note more info in [@main]
 */
void voosh_rebuild_urself(int argc, char **argv, char **envp);

/**
 * @doc!
 * @name(function) build
 * @param `int flags`
 * @desc builds Raiz project
 */
void voosh_build(int flags);

/**
 * @doc!
 * @name(function) run
 * @param `int flags`
 * @param `char **argv`
 * @desc runs the raiz binary with `argv` arguments
 * @note behaviour may change a little based on `flags`
 */
void voosh_run(int flags, char **argv);
