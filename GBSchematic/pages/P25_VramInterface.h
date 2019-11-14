#pragma once

struct Gameboy;

struct P25_VramInterface {

  static void tick(const Gameboy& a, const Gameboy& b, Gameboy& c);

  bool WUKO;
  bool COTA;

private:

  bool MYFU_00,MASA_01,MYRE_02,MAVU_03,MEPA_04,MYSA_05,MEWY_06,MUME_07,VOVA_08,VODE_09,RUKY_10,RUMA_11,REHO_12;
  bool LEXE_00,LOZU_01,LACA_02,LUVO_03,LOLY_04,LALO_05,LEFA_06,LUBY_07,TUJY_08,TAGO_09,NUVA_10,PEDU_11,PONY_12;

  bool RYVO,RERA,RABY,RORY,RUJA,RAVU,RAFY,RUXA;

  union { bool SYRO; bool ADDR_FE00_FFFF; };

  bool CUFE,VAPE,AVER,XUJY,BYCU;
  bool TEFA,SOSE,TUCA,TUJA,TEGU,TAVY,SYCY,SOTO,TUTO,SUDO,TEFY,SALE,TYJY,TOLE;
  bool ROPY,SERE,RUVY,SOHY,SAZO,RYJE,REVO,RELA,ROCY,RENA,ROFA,RAHU;

  bool RYLU,SOHO,RAWA,APAM,SUTU,RACU,RACO,RUTE,SEWO,TODE,SEMA,TAXY,SOFY;
  bool SAHA,SETY,SOKY,REFO,SYSY,RAGU;

  bool RODY_00,REBA_01,RYDO_02,REMO_03,ROCE_04,ROPU_05,RETA_06,RAKU_07;

  bool TUSO,SOLE,RUNY;
  bool TOVU_00,SOSA_01,SEDU_02,TAXO_03,TAHY_04,TESU_05,TAZU_06,TEWA_07;

  bool TEME,TEWU,TYGO,SOTE,SEKE,RUJO,TOFA,SUZA;
  bool SYNU,SYMA,ROKO,SYBU,SAKO,SEJY,SEDO,SAWU;
  bool RURA,RULY,RARE,RODU,RUBE,RUMU,RYTY,RADY;

  bool ROVE;
  bool SEFA_00,SOGO_01,SEFU_02,SUNA_03,SUMO_04,SAZU_05,SAMO_06,SUKE_07;
  bool REGE_00,RYKY_01,RAZO_02,RADA_03,RYRO_04,REVU_05,REKU_06,RYZE_07;

  bool CEDE;
  bool RALO_00,TUNE_01,SERA_02,TENU_03,SYSA_04,SUGY_05,TUBE_06,SYZO_07;
  bool WEJO_00,BUBO_01,BETU_02,CYME_03,BAXU_04,BUHU_05,BYNY_06,BYPY_07;
  bool WASA_00,BOMO_01,BASA_02,CAKO_03,BUMA_04,BUPY_05,BASY_06,BAPE_07;

  bool XANE,XEDU;
  bool XAKY,XUXU,XYNE,XODY,XECA,XOBA,XOPO,XYBO,RYSU,RESE,RUSE,RYNA,RUMO;

  bool TYVY,SEBY;
  bool RERY_00,RUNA_01,RONA_02,RUNO_03,SANA_04,RORO_05,RABO_06,SAME_07;
  bool RUGA_00,ROTA_01,RYBU_02,RAJU_03,TYJA_04,REXU_05,RUPY_06,TOKU_07;

  bool XUCY,XEZE;
  bool XONU_00,WUDO_01,WAWE_02,WOLU_03;

  bool VUZA;
  bool VAPY_04,SEZU_05,VEJY_06,RUSA_07,ROHA_08,RESO_09,SUVO_10,TOBO_11,VURY_12;
};

